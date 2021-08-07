#pragma once
#include <memory>

#include "GlslProgram.h"
#include "GpuTexture.h"
#include "RenderTarget.h"
#include "gl_utils.h"
#include "r_cbuffers.h"

namespace vct {

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
    std::shared_ptr<MeshData> m_box;  // no normals
    MeshData m_quad;

    /// textures
    GpuTexture m_albedoVoxel;
    GpuTexture m_normalVoxel;

    GpuTexture m_lightIcons[MAX_LIGHT_ICON];

    /// render targets
    GBuffer m_gbuffer;
};

}  // namespace vct
