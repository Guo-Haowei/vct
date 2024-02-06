#pragma once
#include "MainRenderer.h"

#include "Core/geometry.h"
#include "Framework/ProgramManager.h"
#include "Framework/SceneManager.h"
#include "core/dynamic_variable/common_dvars.h"
#include "r_defines.h"
#include "r_editor.h"
#include "r_passes.h"
#include "r_rendertarget.h"
#include "r_sun_shadow.h"
#include "servers/display_server.h"

static std::vector<std::shared_ptr<MeshData>> g_meshdata;
static std::vector<std::shared_ptr<MaterialData>> g_materialdata;

extern void FillMaterialCB(const MaterialData* mat, MaterialCB& cb);

namespace vct {

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

void MainRenderer::createGpuResources() {
    R_Create_Pass_Resources();

    R_Alloc_Cbuffers();
    R_CreateEditorResource();
    R_CreateRT();

    Scene& scene = Com_GetScene();

    // create shader

    m_box = CreateMeshData(geometry::MakeBox());

    // create box quad
    R_CreateQuad();

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

    g_constantCache.cache.ShadowMap = gl::MakeTextureResident(g_shadowRT.GetDepthTexture().GetHandle());
    g_constantCache.cache.GbufferDepthMap = gl::MakeTextureResident(g_gbufferRT.GetDepthTexture().GetHandle());
    g_constantCache.cache.GbufferPositionMetallicMap =
        gl::MakeTextureResident(g_gbufferRT.GetColorAttachment(0).GetHandle());
    g_constantCache.cache.GbufferNormalRoughnessMap =
        gl::MakeTextureResident(g_gbufferRT.GetColorAttachment(1).GetHandle());
    g_constantCache.cache.GbufferAlbedoMap = gl::MakeTextureResident(g_gbufferRT.GetColorAttachment(2).GetHandle());
    g_constantCache.cache.VoxelAlbedoMap = gl::MakeTextureResident(m_albedoVoxel.GetHandle());
    g_constantCache.cache.VoxelNormalMap = gl::MakeTextureResident(m_normalVoxel.GetHandle());
    g_constantCache.cache.SSAOMap = gl::MakeTextureResident(g_ssaoRT.GetColorAttachment().GetHandle());
    g_constantCache.cache.FinalImage = gl::MakeTextureResident(g_finalImageRT.GetColorAttachment().GetHandle());
    g_constantCache.cache.FXAA = gl::MakeTextureResident(g_fxaaRT.GetColorAttachment().GetHandle());

    g_constantCache.Update();
}

void MainRenderer::visualizeVoxels() {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    const auto& program = gProgramManager->GetShaderProgram(ProgramType::Visualization);

    glBindVertexArray(m_box->vao);
    program.Bind();

    const int size = DVAR_GET_INT(r_voxelSize);
    glDrawElementsInstanced(GL_TRIANGLES, m_box->count, GL_UNSIGNED_INT, 0, size * size * size);

    program.Unbind();
}

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

void MainRenderer::renderToVoxelTexture() {
    const Scene& scene = Com_GetScene();
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

void MainRenderer::renderFrameBufferTextures(int width, int height) {
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::DebugTexture);

    program.Bind();
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    R_DrawQuad();

    program.Unbind();
}

void MainRenderer::render() {
    g_perFrameCache.Update();

    // clear window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    R_ShadowPass();

    // if (scene.dirty || DVAR_GET_BOOL(r_forceVXGI))
    {
        m_albedoVoxel.clear();
        m_normalVoxel.clear();
        renderToVoxelTexture();
    }

    auto [frameW, frameH] = DisplayServerGLFW::singleton().get_frame_size();
    if (frameW * frameH > 0) {
        // skip rendering if minimized
        glViewport(0, 0, frameW, frameH);

        R_Gbuffer_Pass();
        R_SSAO_Pass();

        const int mode = DVAR_GET_INT(r_debugTexture);

        switch (mode) {
            case DrawTexture::TEXTURE_VOXEL_ALBEDO:
            case DrawTexture::TEXTURE_VOXEL_NORMAL:
                visualizeVoxels();
                R_DrawEditor();
                break;
            default: {
                R_Deferred_VCT_Pass();
                R_FXAA_Pass();

                g_viewerRT.Bind();
                renderFrameBufferTextures(frameW, frameH);
                R_DrawEditor();
                g_viewerRT.Unbind();
            } break;
        }
    }
}

void MainRenderer::destroyGpuResources() {
    R_DestroyRT();

    R_DestroyEditorResource();
    R_Destroy_Cbuffers();

    R_Destroy_Pass_Resources();
}

}  // namespace vct
