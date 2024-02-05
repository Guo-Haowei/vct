#include "r_passes.h"

#include <random>

#include "Core/CommonDvars.h"
#include "Core/camera.h"
#include "Framework/ProgramManager.h"
#include "Framework/SceneManager.h"
#include "Framework/WindowManager.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "r_rendertarget.h"
/////////////////////
#include "core/math/frustum.h"

static GLuint g_noiseTexture;

extern void FillMaterialCB(const MaterialData* mat, MaterialCB& cb);

void R_Gbuffer_Pass() {
    Scene& scene = Com_GetScene();
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::GBUFFER);

    g_gbufferRT.Bind();
    program.Bind();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Frustum frustum(gCamera.ProjView());

    const uint32_t numObjects = (uint32_t)scene.GetCount<ObjectComponent>();
    for (uint32_t i = 0; i < numObjects; ++i) {
        const ObjectComponent& obj = scene.GetComponentArray<ObjectComponent>()[i];
        ecs::Entity entity = scene.GetEntity<ObjectComponent>(i);
        DEV_ASSERT(scene.Contains<TransformComponent>(entity));
        const TransformComponent& transform = *scene.GetComponent<TransformComponent>(entity);
        DEV_ASSERT(scene.Contains<MeshComponent>(obj.meshID));
        const MeshComponent& mesh = *scene.GetComponent<MeshComponent>(obj.meshID);

        const mat4& M = transform.GetWorldMatrix();
        AABB aabb = mesh.mLocalBound;
        aabb.apply_matrix(M);
        if (!frustum.intersects(aabb)) {
            continue;
        }

        g_perBatchCache.cache.Model = M;
        g_perBatchCache.cache.PVM = g_perFrameCache.cache.PV * M;
        g_perBatchCache.Update();

        const MeshData* drawData = reinterpret_cast<MeshData*>(mesh.gpuResource);
        glBindVertexArray(drawData->vao);

        for (const auto& subset : mesh.mSubsets) {
            aabb = subset.localBound;
            aabb.apply_matrix(M);
            if (!frustum.intersects(aabb)) {
                continue;
            }

            const MaterialComponent& material = *scene.GetComponent<MaterialComponent>(subset.materialID);
            const MaterialData* matData = reinterpret_cast<MaterialData*>(material.gpuResource);

            FillMaterialCB(matData, g_materialCache.cache);
            g_materialCache.Update();

            glDrawElements(GL_TRIANGLES, subset.indexCount, GL_UNSIGNED_INT,
                           (void*)(subset.indexOffset * sizeof(uint32_t)));
        }
    }

    program.Unbind();
    g_gbufferRT.Unbind();
}

void R_SSAO_Pass() {
    const auto& shader = gProgramManager->GetShaderProgram(ProgramType::SSAO);

    g_ssaoRT.Bind();

    glClear(GL_COLOR_BUFFER_BIT);

    shader.Bind();

    R_DrawQuad();

    shader.Unbind();

    g_ssaoRT.Unbind();
}

void R_Deferred_VCT_Pass() {
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::VCT_DEFERRED);
    g_finalImageRT.Bind();

    glClear(GL_COLOR_BUFFER_BIT);

    program.Bind();

    R_DrawQuad();

    program.Unbind();

    g_finalImageRT.Unbind();
}

void R_FXAA_Pass() {
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::FXAA);

    g_fxaaRT.Bind();
    glClear(GL_COLOR_BUFFER_BIT);

    program.Bind();
    R_DrawQuad();
    program.Unbind();

    g_fxaaRT.Unbind();
}

static float lerp(float a, float b, float f) { return a + f * (b - a); }

static void CreateSSAOResource() {
    // generate sample kernel
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);  // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec4> ssaoKernel;
    const int kernelSize = DVAR_GET_INT(r_ssaoKernelSize);
    for (int i = 0; i < kernelSize; ++i) {
        // [-1, 1], [-1, 1], [0, 1]
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / kernelSize;

        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.emplace_back(vec4(sample, 0.0f));
    }

    memset(&g_constantCache.cache.SSAOKernels, 0, sizeof(g_constantCache.cache.SSAOKernels));
    memcpy(&g_constantCache.cache.SSAOKernels, ssaoKernel.data(), sizeof(ssaoKernel.front()) * ssaoKernel.size());

    // generate noise texture
    const int noiseSize = DVAR_GET_INT(r_ssaoNoiseSize);

    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < noiseSize * noiseSize; ++i) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
        noise = glm::normalize(noise);
        ssaoNoise.emplace_back(noise);
    }
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    g_constantCache.cache.NoiseMap = gl::MakeTextureResident(noiseTexture);
    g_noiseTexture = noiseTexture;
}

void R_Create_Pass_Resources() { CreateSSAOResource(); }

void R_Destroy_Pass_Resources() { glDeleteTextures(1, &g_noiseTexture); }
