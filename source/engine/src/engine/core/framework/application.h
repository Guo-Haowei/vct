#pragma once
#include "core/framework/event_queue.h"
#include "core/framework/layer.h"
#include "core/os/os.h"

namespace vct {

class DisplayServer;
class RenderingServer;
class SceneManager;

class Application {
public:
    int run(int argc, const char** argv);

    virtual void init_layers(){};

    EventQueue& get_event_queue() { return m_event_queue; }

protected:
    void add_layer(std::shared_ptr<Layer> layer);

private:
    void save_command_line(int argc, const char** argv);

    std::vector<std::shared_ptr<Layer>> m_layers;
    std::vector<std::string> m_command_line;
    std::string m_app_name;

    std::shared_ptr<OS> m_os;

    EventQueue m_event_queue;

    std::shared_ptr<DisplayServer> m_display_server;
    std::shared_ptr<RenderingServer> m_rendering_server;
    std::shared_ptr<SceneManager> m_scene_manager;
};

}  // namespace vct