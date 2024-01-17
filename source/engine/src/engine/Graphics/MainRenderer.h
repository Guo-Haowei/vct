#pragma once
#include "GpuTexture.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "r_rendertarget.h"

namespace vct
{

class MainRenderer
{
public:
    void createGpuResources();
    void render();
    void renderFrameBufferTextures(int width, int height);
    void renderToVoxelTexture();
    void visualizeVoxels();
    void destroyGpuResources();

    void gbufferPass();
    void vctPass();

private:
    /// vertex arrays
    std::shared_ptr<MeshData> m_box;  // no normals

    /// textures
    GpuTexture m_albedoVoxel;
    GpuTexture m_normalVoxel;

    GpuTexture m_lightIcons[MAX_LIGHT_ICON];
};

}  // namespace vct
