#pragma once
#include "core/objects/singleton.h"
#include "rendering/GpuTexture.h"
#include "rendering/gl_utils.h"
#include "rendering/r_cbuffers.h"
#include "scene/scene_listener.h"

namespace vct {

struct RenderData;

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

    void begin_scene(Scene& scene) override;

    void createGpuResources();
    void render();
    void destroyGpuResources();

    uint32_t get_final_image() const;

    std::shared_ptr<RenderData> get_render_data() { return m_render_data; }

private:
    GpuTexture m_lightIcons[MAX_LIGHT_ICON];
    int m_method = RENDER_GRAPH_NONE;

    std::shared_ptr<RenderData> m_render_data;
};

}  // namespace vct
