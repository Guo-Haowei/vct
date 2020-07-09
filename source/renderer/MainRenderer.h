#pragma once
#include "GlslProgram.h"
#include "GpuTexture.h"
#include "GpuBuffer.h"
#include "RenderTarget.h"
#include "application/Window.h"

namespace vct {

struct MeshData
{
    GLuint vao          = 0;
    GLuint ebo          = 0;
    GLuint vbos[5]      = { 0, 0, 0, 0, 0 };
    unsigned int count  = 0;
};

struct MaterialData
{
    GpuTexture albedoMap;
    GpuTexture materialMap;
    GpuTexture normalMap;
    Vector4 albedoColor;
    float metallic;
    float roughness;
    // specular...
    // normal...
};

struct CameraBufferCache
{
    Matrix4 PV;
    // Vector3 position;
    // float padding1;
};

struct LightBufferCache
{
    Vector3 position;
    float padding;
    Matrix4 lightSpacePV;
};

struct MaterialCache
{
    Vector4 albedo_color; // if it doesn't have albedo color, then it's alpha is 0.0f
    float metallic = 0.0f;
    float roughness = 0.0f;
    float has_metallic_roughness_texture = 0.0f;
    float has_normal_texture = 0.0f;

    MaterialCache& operator=(const MaterialData& mat)
    {
        albedo_color = mat.albedoColor;
        roughness = mat.roughness;
        metallic = mat.metallic;
        has_metallic_roughness_texture = mat.materialMap.getHandle() == 0 ? 0.0f : 1.0f;
        has_normal_texture = mat.normalMap.getHandle() == 0 ? 0.0f : 1.0f;

        return *this;
    }
};

static_assert(sizeof(CameraBufferCache) % 16 == 0);
static_assert(sizeof(LightBufferCache) % 16 == 0);
static_assert(sizeof(MaterialCache) % 16 == 0);

class MainRenderer
{
public:
    void createGpuResources();
    void createFrameBuffers();
    void render();
    // void renderToEarlyZ(const Matrix4& PV);
    void renderFrameBufferTextures(const Extent2i& extent);
    void renderToVoxelTexture();
    void renderBoundingBox();
    void visualizeVoxels();
    void renderSceneNoGI();
    void renderSceneVCT();
    void destroyGpuResources();

    void gbufferPass();
    void shadowPass();

    inline void setWindow(Window* pWindow) { m_pWindow = pWindow; }
private:
    Window* m_pWindow;

    /// shader programs
    GlslProgram m_voxelProgram;
    GlslProgram m_visualizeProgram;
    GlslProgram m_basicProgram;
    GlslProgram m_vctProgram;
    GlslProgram m_boxWireframeProgram;
    GlslProgram m_voxelPostProgram;
    GlslProgram m_depthProgram;
    GlslProgram m_debugTextureProgram;

    GlslProgram m_gbufferProgram;

    /// vertex arrays
    MeshData m_boxWireframe;
    MeshData m_box; // no normals
    MeshData m_quad;

    /// textures
    GpuTexture m_albedoVoxel;
    GpuTexture m_normalVoxel;

    /// uniform buffers
    UniformBuffer<CameraBufferCache>    m_cameraBuffer;
    UniformBuffer<LightBufferCache>     m_lightBuffer;
    UniformBuffer<MaterialCache>        m_materialBuffer;

    /// render targets
    DepthRenderTarget                   m_shadowBuffer;
    GBuffer                             m_gbuffer;
};

} // namespace vct
