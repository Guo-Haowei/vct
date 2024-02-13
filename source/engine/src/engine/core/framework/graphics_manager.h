#pragma once
#include "core/base/singleton.h"
#include "core/framework/event_queue.h"
#include "core/framework/module.h"
#include "rendering/GpuTexture.h"
#include "rendering/gl_utils.h"
#include "rendering/r_cbuffers.h"

namespace vct {

struct RenderData;

class GraphicsManager : public Singleton<GraphicsManager>, public Module, public EventListener {
public:
    enum {
        RENDER_GRAPH_NONE,
        RENDER_GRAPH_VXGI,
        RENDER_GRAPH_VXGI_DEBUG,
    };

    GraphicsManager() : Module("GraphicsManager") {}

    bool initialize();
    void finalize();

    void event_received(std::shared_ptr<Event> event) override;

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
