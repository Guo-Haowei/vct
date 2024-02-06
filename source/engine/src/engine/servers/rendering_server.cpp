#pragma once
#include "rendering_server.h"

#include <random>

#include "Core/geometry.h"
#include "Framework/ProgramManager.h"
#include "imgui/backends/imgui_impl_opengl3.h"
/////
#include "core/dynamic_variable/common_dvars.h"
#include "rendering/passes.h"
#include "rendering/r_defines.h"
#include "rendering/r_editor.h"
#include "scene/scene_manager.h"
#include "servers/display_server.h"

static std::vector<std::shared_ptr<MeshData>> g_meshdata;
static std::vector<std::shared_ptr<MaterialData>> g_materialdata;

static GLuint g_noiseTexture;

extern void FillMaterialCB(const MaterialData* mat, MaterialCB& cb);

namespace vct {

static void APIENTRY gl_debug_callback(GLenum, GLenum, unsigned int, GLenum, GLsizei, const char*, const void*);

bool RenderingServer::initialize() {
    if (gladLoadGL() == 0) {
        LOG_FATAL("[glad] failed to load gl functions");
        return false;
    }

    LOG_VERBOSE("[opengl] renderer: {}", (const char*)glGetString(GL_RENDERER));
    LOG_VERBOSE("[opengl] version: {}", (const char*)glGetString(GL_VERSION));

    if (DVAR_GET_BOOL(r_gpu_validation)) {
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(gl_debug_callback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            LOG_VERBOSE("[opengl] debug callback enabled");
        }
    }

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    createGpuResources();
    return true;
}

void RenderingServer::finalize() {
    destroyGpuResources();

    ImGui_ImplOpenGL3_Shutdown();
}

static std::shared_ptr<MeshData> CreateMeshData(const MeshComponent& mesh) {
    MeshData* ret = new MeshData;

    MeshData& outMesh = *ret;
    const bool hasNormals = !mesh.mNormals.empty();
    const bool hasUVs = !mesh.mTexcoords_0.empty();
    const bool hasTangent = !mesh.mTangents.empty();
    const bool hasBitangent = !mesh.mBitangents.empty();

    glGenVertexArrays(1, &outMesh.vao);
    glGenBuffers(2 + hasNormals + hasUVs + hasTangent + hasBitangent, &outMesh.ebo);
    glBindVertexArray(outMesh.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outMesh.ebo);
    gl::BindToSlot(outMesh.vbos[0], 0, 3);
    gl::NamedBufferStorage(outMesh.vbos[0], mesh.mPositions);
    if (hasNormals) {
        gl::BindToSlot(outMesh.vbos[1], 1, 3);
        gl::NamedBufferStorage(outMesh.vbos[1], mesh.mNormals);
    }
    if (hasUVs) {
        gl::BindToSlot(outMesh.vbos[2], 2, 2);
        gl::NamedBufferStorage(outMesh.vbos[2], mesh.mTexcoords_0);
    }
    if (hasTangent) {
        gl::BindToSlot(outMesh.vbos[3], 3, 3);
        gl::NamedBufferStorage(outMesh.vbos[3], mesh.mTangents);
        gl::BindToSlot(outMesh.vbos[4], 4, 3);
        gl::NamedBufferStorage(outMesh.vbos[4], mesh.mBitangents);
    }

    gl::NamedBufferStorage(outMesh.ebo, mesh.mIndices);
    outMesh.count = static_cast<uint32_t>(mesh.mIndices.size());

    glBindVertexArray(0);
    return std::shared_ptr<MeshData>(ret);
}

void RenderingServer::begin_scene(Scene& scene) {
    // create mesh
    for (const auto& mesh : scene.GetComponentArray<MeshComponent>()) {
        g_meshdata.emplace_back(CreateMeshData(mesh));
        mesh.gpuResource = g_meshdata.back().get();
    }

    // create material
    DEV_ASSERT(scene.GetCount<MaterialComponent>() < array_length(g_constantCache.cache.AlbedoMaps));

    for (int idx = 0; idx < scene.GetCount<MaterialComponent>(); ++idx) {
        const auto& mat = scene.GetComponentArray<MaterialComponent>()[idx];

        auto matData = std::make_shared<MaterialData>();

        {
            const std::string& textureMap = mat.mTextures[MaterialComponent::Base].name;
            matData->albedoColor = mat.mBaseColor;
            if (!textureMap.empty()) {
                matData->albedoMap.Create2DImageFromFile(textureMap);
                g_constantCache.cache.AlbedoMaps[idx].data = gl::MakeTextureResident(matData->albedoMap.GetHandle());
            }
        }

        {
            const std::string& textureMap = mat.mTextures[MaterialComponent::MetallicRoughness].name;
            matData->metallic = mat.mMetallic;
            matData->roughness = mat.mRoughness;
            if (!textureMap.empty()) {
                matData->materialMap.Create2DImageFromFile(textureMap);
                g_constantCache.cache.PbrMaps[idx].data = gl::MakeTextureResident(matData->materialMap.GetHandle());
            }
        }

        {
            const std::string& textureMap = mat.mTextures[MaterialComponent::Normal].name;
            if (!textureMap.empty()) {
                matData->normalMap.Create2DImageFromFile(textureMap);
                g_constantCache.cache.NormalMaps[idx].data = gl::MakeTextureResident(matData->normalMap.GetHandle());
                // LOG("material has bump {}", mat->normalTexture.c_str());
            }
        }

        matData->textureMapIdx = idx;

        g_materialdata.emplace_back(matData);
        mat.gpuResource = g_materialdata.back().get();
    }

    g_constantCache.Update();
}

static void create_ssao_resource() {
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

void RenderingServer::createGpuResources() {
    create_ssao_resource();

    R_Alloc_Cbuffers();
    R_CreateEditorResource();

    create_passes();

    const int voxelSize = DVAR_GET_INT(r_voxelSize);

    /// create voxel image
    {
        Texture3DCreateInfo info;
        info.wrapS = info.wrapT = info.wrapR = GL_CLAMP_TO_BORDER;
        info.size = voxelSize;
        info.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        info.magFilter = GL_NEAREST;
        info.mipLevel = log_two(voxelSize);
        info.format = GL_RGBA16F;

        m_albedoVoxel.create3DEmpty(info);
        m_normalVoxel.create3DEmpty(info);
    }

    // create box quad
    R_CreateQuad();

    g_constantCache.cache.VoxelAlbedoMap = gl::MakeTextureResident(m_albedoVoxel.GetHandle());
    g_constantCache.cache.VoxelNormalMap = gl::MakeTextureResident(m_normalVoxel.GetHandle());

    g_constantCache.cache.FXAA =
        gl::MakeTextureResident(g_fxaa_rt.get_color_attachment(0));

    g_constantCache.cache.ShadowMap =
        gl::MakeTextureResident(g_shadow_rt.get_depth_attachment());

    g_constantCache.cache.SSAOMap =
        gl::MakeTextureResident(g_ssao_rt.get_color_attachment(0));
    g_constantCache.cache.FinalImage =
        gl::MakeTextureResident(g_final_image_rt.get_color_attachment(0));

    g_constantCache.cache.GbufferDepthMap =
        gl::MakeTextureResident(g_gbuffer_rt.get_depth_attachment());
    g_constantCache.cache.GbufferPositionMetallicMap =
        gl::MakeTextureResident(g_gbuffer_rt.get_color_attachment(0));
    g_constantCache.cache.GbufferNormalRoughnessMap =
        gl::MakeTextureResident(g_gbuffer_rt.get_color_attachment(1));
    g_constantCache.cache.GbufferAlbedoMap =
        gl::MakeTextureResident(g_gbuffer_rt.get_color_attachment(2));

    g_constantCache.Update();

    // on_scene_change();
}

// void MainRenderer::visualizeVoxels() {
//  glEnable(GL_CULL_FACE);
//  glEnable(GL_DEPTH_TEST);

// const auto& program = gProgramManager->GetShaderProgram(ProgramType::Visualization);

// glBindVertexArray(m_box->vao);
// program.Bind();

// const int size = DVAR_GET_INT(r_voxelSize);
// glDrawElementsInstanced(GL_TRIANGLES, m_box->count, GL_UNSIGNED_INT, 0, size * size * size);

// program.Unbind();
//}

struct MaterialCache {
    vec4 albedo_color;  // if it doesn't have albedo color, then it's alpha is 0.0f
    float metallic = 0.0f;
    float roughness = 0.0f;
    float has_metallic_roughness_texture = 0.0f;
    float has_normal_texture = 0.0f;
    float reflect = 0.0f;

    MaterialCache& operator=(const MaterialData& mat) {
        albedo_color = mat.albedoColor;
        roughness = mat.roughness;
        metallic = mat.metallic;
        reflect = mat.reflectPower;
        has_metallic_roughness_texture = mat.materialMap.GetHandle() == 0 ? 0.0f : 1.0f;
        has_normal_texture = mat.normalMap.GetHandle() == 0 ? 0.0f : 1.0f;

        return *this;
    }
};

void RenderingServer::renderToVoxelTexture() {
    const Scene& scene = SceneManager::get_scene();
    const int voxelSize = DVAR_GET_INT(r_voxelSize);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glViewport(0, 0, voxelSize, voxelSize);

    m_albedoVoxel.bindImageTexture(IMAGE_VOXEL_ALBEDO_SLOT);
    m_normalVoxel.bindImageTexture(IMAGE_VOXEL_NORMAL_SLOT);
    gProgramManager->GetShaderProgram(ProgramType::Voxel).Bind();

    const uint32_t numObjects = (uint32_t)scene.GetCount<ObjectComponent>();
    for (uint32_t i = 0; i < numObjects; ++i) {
        const ObjectComponent& obj = scene.GetComponentArray<ObjectComponent>()[i];
        ecs::Entity entity = scene.GetEntity<ObjectComponent>(i);
        DEV_ASSERT(scene.Contains<TransformComponent>(entity));
        const TransformComponent& transform = *scene.GetComponent<TransformComponent>(entity);
        DEV_ASSERT(scene.Contains<MeshComponent>(obj.meshID));
        const MeshComponent& mesh = *scene.GetComponent<MeshComponent>(obj.meshID);

        const mat4& M = transform.GetWorldMatrix();
        g_perBatchCache.cache.Model = M;
        g_perBatchCache.cache.PVM = g_perFrameCache.cache.PV * M;
        g_perBatchCache.Update();

        const MeshData* drawData = reinterpret_cast<MeshData*>(mesh.gpuResource);
        glBindVertexArray(drawData->vao);

        for (const auto& subset : mesh.mSubsets) {
            const MaterialComponent& material = *scene.GetComponent<MaterialComponent>(subset.materialID);
            const MaterialData* matData = reinterpret_cast<MaterialData*>(material.gpuResource);

            FillMaterialCB(matData, g_materialCache.cache);
            g_materialCache.Update();

            glDrawElements(GL_TRIANGLES, subset.indexCount, GL_UNSIGNED_INT,
                           (void*)(subset.indexOffset * sizeof(uint32_t)));
        }
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // post process
    gProgramManager->GetShaderProgram(ProgramType::VoxelPost).Bind();

    constexpr GLuint workGroupX = 512;
    constexpr GLuint workGroupY = 512;
    const GLuint workGroupZ = (voxelSize * voxelSize * voxelSize) / (workGroupX * workGroupY);

    glDispatchCompute(workGroupX, workGroupY, workGroupZ);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_albedoVoxel.bind();
    m_albedoVoxel.genMipMap();
    m_normalVoxel.bind();
    m_normalVoxel.genMipMap();
}

void RenderingServer::renderFrameBufferTextures(int width, int height) {
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::DebugTexture);

    program.Bind();
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    R_DrawQuad();

    program.Unbind();
}

void RenderingServer::render() {
    check_scene_update();

    g_perFrameCache.Update();

    // clear window

    g_shadow_rt.execute();

    // @TODO: make it a pass
    {
        m_albedoVoxel.clear();
        m_normalVoxel.clear();
        renderToVoxelTexture();
    }

    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();
    if (frameW * frameH > 0) {
        // skip rendering if minimized
        glViewport(0, 0, frameW, frameH);

        g_gbuffer_rt.execute();
        g_ssao_rt.execute();
        g_final_image_rt.execute();
        g_fxaa_rt.execute();

        // @TODO: make it a pass
        g_viewer_rt.bind();
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderFrameBufferTextures(frameW, frameH);
        R_DrawEditor();
        g_viewer_rt.unbind();
    }
}

void RenderingServer::destroyGpuResources() {
    destroy_passes();

    R_DestroyEditorResource();
    R_Destroy_Cbuffers();

    glDeleteTextures(1, &g_noiseTexture);
}

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                                       const char* message, const void* userParam) {
    vct::unused(length);
    vct::unused(userParam);

    switch (id) {
        case 131185:
            return;
    }

    const char* sourceStr = "GL_DEBUG_SOURCE_OTHER";
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            sourceStr = "GL_DEBUG_SOURCE_API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceStr = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceStr = "GL_DEBUG_SOURCE_SHADER_COMPILER";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceStr = "GL_DEBUG_SOURCE_THIRD_PARTY";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            sourceStr = "GL_DEBUG_SOURCE_APPLICATION";
            break;
        default:
            break;
    }

    const char* typeStr = "GL_DEBUG_TYPE_OTHER";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            typeStr = "GL_DEBUG_TYPE_ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeStr = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeStr = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeStr = "GL_DEBUG_TYPE_PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeStr = "GL_DEBUG_TYPE_PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_MARKER:
            typeStr = "GL_DEBUG_TYPE_MARKER";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            typeStr = "GL_DEBUG_TYPE_PUSH_GROUP";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            typeStr = "GL_DEBUG_TYPE_POP_GROUP";
            break;
        default:
            break;
    }

    LogLevel level = LOG_LEVEL_NORMAL;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            level = LOG_LEVEL_ERROR;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            level = LOG_LEVEL_WARN;
            break;
        default:
            break;
    }

    vct::log_impl(level, std::format("[opengl] {}\n\t| id: {} | source: {} | type: {}", message, id, sourceStr, typeStr));
}

}  // namespace vct
