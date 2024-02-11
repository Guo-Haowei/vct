#pragma once
#include "core/objects/singleton.h"
#include "rendering/GpuTexture.h"
#include "rendering/gl_utils.h"
#include "rendering/passes.h"
#include "rendering/r_cbuffers.h"
#include "scene/scene_listener.h"

namespace vct {

class RenderingServer : public Singleton<RenderingServer>, public SceneListener {
public:
    RenderingServer() : SceneListener("renderer") {}

    bool initialize();
    void finalize();

    virtual void begin_scene(Scene& scene) override;

    void createGpuResources();
    void render();
    void renderFrameBufferTextures(int width, int height);
    void destroyGpuResources();

    void gbufferPass();
    void vctPass();

private:
    /// vertex arrays

    GpuTexture m_lightIcons[MAX_LIGHT_ICON];
};

}  // namespace vct
