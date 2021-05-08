#pragma once
#include "GlslProgram.h"
#include "GpuBuffer.h"
#include "GpuTexture.h"
#include "RenderTarget.h"
#include "common/Window.h"

namespace vct {

struct MeshData {
    GLuint vao         = 0;
    GLuint ebo         = 0;
    GLuint vbos[5]     = { 0, 0, 0, 0, 0 };
    unsigned int count = 0;
};

struct MaterialData {
    GpuTexture albedoMap;
    GpuTexture materialMap;
    GpuTexture normalMap;
    vec4 albedoColor;
    float metallic;
    float roughness;
    // specular...
    // normal...
};

struct VSPerFrameCache {
    mat4 PV;
    mat4 lightSpace;
};

struct FSPerFrameCache {
    vec3 light_position;  // direction
    float _per_frame_pad0;
    vec3 light_color;
    float _per_frame_pad1;
    vec3 camera_position;
    float _per_frame_pad2;
};

struct MaterialCache {
    vec4 albedo_color;  // if it doesn't have albedo color, then it's alpha is 0.0f
    float metallic                       = 0.0f;
    float roughness                      = 0.0f;
    float has_metallic_roughness_texture = 0.0f;
    float has_normal_texture             = 0.0f;

    MaterialCache& operator=( const MaterialData& mat )
    {
        albedo_color                   = mat.albedoColor;
        roughness                      = mat.roughness;
        metallic                       = mat.metallic;
        has_metallic_roughness_texture = mat.materialMap.getHandle() == 0 ? 0.0f : 1.0f;
        has_normal_texture             = mat.normalMap.getHandle() == 0 ? 0.0f : 1.0f;

        return *this;
    }
};

struct ConstantCache {
    vec3 world_center;
    float world_size_half;
    float texel_size;
    float voxel_size;
    float padding[2];
};

static_assert( sizeof( VSPerFrameCache ) % 16 == 0 );
static_assert( sizeof( FSPerFrameCache ) % 16 == 0 );
static_assert( sizeof( MaterialCache ) % 16 == 0 );
static_assert( sizeof( ConstantCache ) % 16 == 0 );

class MainRenderer {
   public:
    void createGpuResources();
    void createFrameBuffers();
    void render();
    void renderFrameBufferTextures( const ivec2& extent );
    void renderToVoxelTexture();
    void renderBoundingBox();
    void visualizeVoxels();
    void destroyGpuResources();

    void gbufferPass();
    void shadowPass();
    void vctPass();

    inline void setWindow( Window* pWindow ) { m_pWindow = pWindow; }

   private:
    Window* m_pWindow;

    /// shader programs
    GlslProgram m_voxelProgram;
    GlslProgram m_visualizeProgram;
    GlslProgram m_vctProgram;
    GlslProgram m_boxWireframeProgram;
    GlslProgram m_voxelPostProgram;
    GlslProgram m_depthProgram;
    GlslProgram m_debugTextureProgram;

    GlslProgram m_gbufferProgram;

    /// vertex arrays
    MeshData m_boxWireframe;
    MeshData m_box;  // no normals
    MeshData m_quad;

    /// textures
    GpuTexture m_albedoVoxel;
    GpuTexture m_normalVoxel;

    /// uniform buffers
    UniformBuffer<VSPerFrameCache> m_vsPerFrameBuffer;  // global binding 0
    UniformBuffer<FSPerFrameCache> m_fsPerFrameBuffer;  // global binding 1
    UniformBuffer<MaterialCache> m_fsMaterialBuffer;    // global binding 2
    UniformBuffer<ConstantCache> m_constantBuffer;      // global binding 3

    /// render targets
    DepthRenderTarget m_shadowBuffer;
    GBuffer m_gbuffer;
};

}  // namespace vct
