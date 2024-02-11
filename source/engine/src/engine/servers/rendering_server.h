#pragma once
#include "core/objects/singleton.h"
#include "rendering/GpuTexture.h"
#include "rendering/gl_utils.h"
#include "rendering/r_cbuffers.h"
#include "scene/scene_listener.h"

namespace vct {

class RenderingServer : public Singleton<RenderingServer>, public SceneListener {
public:
    enum {
        RENDER_GRAPH_NONE,
        RENDER_GRAPH_VXGI,
        RENDER_GRAPH_VXGI_DEBUG,
    };

    RenderingServer() : SceneListener("renderer") {}

    bool initialize();
    void finalize();

    virtual void begin_scene(Scene& scene) override;

    void createGpuResources();
    void render();
    void destroyGpuResources();

    uint32_t get_final_image() const;

private:
    GpuTexture m_lightIcons[MAX_LIGHT_ICON];
    int m_method = RENDER_GRAPH_NONE;
};

}  // namespace vct
