#pragma once
#include "GlslProgram.h"
#include "GpuTexture.h"
#include "RenderTarget.h"
#include "gl_utils.h"
#include "r_cbuffers.h"

namespace vct {

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
        has_metallic_roughness_texture = mat.materialMap.GetHandle() == 0 ? 0.0f : 1.0f;
        has_normal_texture             = mat.normalMap.GetHandle() == 0 ? 0.0f : 1.0f;

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

static_assert( sizeof( MaterialCache ) % 16 == 0 );
static_assert( sizeof( ConstantCache ) % 16 == 0 );

class MainRenderer {
   public:
    void createGpuResources();
    void createFrameBuffers();
    void render();
    void renderFrameBufferTextures( const ivec2& extent );
    void renderToVoxelTexture();
    void visualizeVoxels();
    void destroyGpuResources();

    void gbufferPass();
    void vctPass();

   private:
    GlslProgram m_voxelProgram;
    GlslProgram m_visualizeProgram;
    GlslProgram m_vctProgram;
    GlslProgram m_voxelPostProgram;
    GlslProgram m_debugTextureProgram;

    GlslProgram m_gbufferProgram;

    /// vertex arrays
    MeshData m_box;  // no normals
    MeshData m_quad;

    /// textures
    GpuTexture m_albedoVoxel;
    GpuTexture m_normalVoxel;

    /// uniform buffers
    gl::ConstantBuffer<MaterialCache> m_fsMaterialBuffer;  // global binding 2
    gl::ConstantBuffer<ConstantCache> m_constantBuffer;    // global binding 3

    /// render targets
    GBuffer m_gbuffer;
};

}  // namespace vct
