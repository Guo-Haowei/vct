#pragma once
#include "GpuTexture.h"
#include "gl_utils.h"
#include "passes.h"
#include "r_cbuffers.h"

#include "Framework/scene_listener.h"

namespace vct {

class MainRenderer : public SceneListener {
public:
    MainRenderer() : SceneListener("renderer") {}

    virtual void begin_scene() override;

    void createGpuResources();
    void render();
    void renderFrameBufferTextures(int width, int height);
    void renderToVoxelTexture();
    void destroyGpuResources();

    void gbufferPass();
    void vctPass();

private:
    /// vertex arrays

    /// textures
    GpuTexture m_albedoVoxel;
    GpuTexture m_normalVoxel;

    GpuTexture m_lightIcons[MAX_LIGHT_ICON];
};

}  // namespace vct
