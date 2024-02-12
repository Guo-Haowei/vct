#include "application.h"

#include "imgui/imgui.h"
// @TODO: refactor

#include "assets/asset_loader.h"
#include "core/dynamic_variable/common_dvars.h"
#include "core/dynamic_variable/dynamic_variable_manager.h"
#include "core/framework/UIManager.h"
#include "core/input/input.h"
#include "core/os/threads.h"
#include "core/systems/job_system.h"
#include "core/utility/timer.h"
#include "scene/scene_manager.h"
#include "servers/display_server.h"
#include "servers/display_server_glfw.h"
#include "servers/rendering/rendering_dvars.h"
#include "servers/rendering/rendering_misc.h"
#include "servers/rendering/shader_program_manager.h"
#include "servers/rendering_server.h"

namespace vct {

// @TODO: refactor
static vct::DisplayServer* s_display_server = new vct::DisplayServerGLFW;
static vct::RenderingServer* s_rendering_server = new vct::RenderingServer;

static void register_common_dvars() {
#define REGISTER_DVAR
#include "core/dynamic_variable/common_dvars.h"
}

void Application::add_layer(std::shared_ptr<Layer> layer) {
    m_layers.emplace_back(layer);
}

void Application::save_command_line(int argc, const char** argv) {
    m_app_name = argv[0];
    for (int i = 1; i < argc; ++i) {
        m_command_line.push_back(argv[i]);
    }
}

int Application::run(int argc, const char** argv) {
    m_os = std::make_shared<OS>();

    save_command_line(argc, argv);

    // intialize
    OS::singleton().initialize();

    // dvars
    register_common_dvars();
    register_rendering_dvars();
    DynamicVariableManager::parse(m_command_line);
    DynamicVariableManager::deserialize();

    thread::initialize();
    jobsystem::initialize();
    asset_loader::initialize();

    SceneManager::singleton().initialize();
    UIManager::initialize();
    DisplayServer::singleton().initialize();
    RenderingServer::singleton().initialize();
    ShaderProgramManager::singleton().initialize();

    init_layers();
    for (auto& layer : m_layers) {
        layer->Attach();
        LOG("[Runtime] layer '{}' attached!", layer->GetName());
    }

    LOG("\n********************************************************************************"
        "\nMain Loop"
        "\n********************************************************************************");

    LOG_ERROR("TODO: physics");
    LOG_WARN("TODO: load lua scene");
    LOG_ERROR("TODO: FXAA");
    LOG_WARN("TODO: TAA");
    LOG_ERROR("TODO: soft shadow");
    LOG_WARN("TODO: path tracer here");
    LOG_ERROR("TODO: migrate PBR code here");
    LOG_WARN("TODO: fix multiple objects play same animation");
    LOG_ERROR("TODO: link object with animation");
    LOG_WARN("TODO: render texels around camera");
    LOG_ERROR("TODO: cloth physics");
    LOG_WARN("TODO: refactor application");

    // @TODO: add frame count, elapsed time, etc
    Timer timer;
    while (!DisplayServer::singleton().should_close()) {
        DisplayServer::singleton().new_frame();

        input::begin_frame();

        // @TODO: better elapsed time
        float dt = static_cast<float>(timer.get_duration().to_second());
        dt = glm::min(dt, 0.1f);
        SceneManager::singleton().update(dt);
        timer.start();

        ImGui::NewFrame();
        for (auto& layer : m_layers) {
            layer->Update(dt);
        }

        for (auto& layer : m_layers) {
            layer->Render();
        }
        ImGui::Render();

        RenderingServer::singleton().render();

        DisplayServer::singleton().present();

        ImGui::EndFrame();

        input::end_frame();
    }

    LOG("\n********************************************************************************"
        "\nMain Loop"
        "\n********************************************************************************");

    // @TODO: fix
    auto [w, h] = DisplayServer::singleton().get_frame_size();
    DVAR_SET_IVEC2(window_resolution, w, h);
    auto [x, y] = DisplayServer::singleton().get_window_pos();
    DVAR_SET_IVEC2(window_position, x, y);

    // @TODO: move it to request shutdown
    thread::request_shutdown();

    // finalize
    ShaderProgramManager::singleton().finalize();
    RenderingServer::singleton().finalize();
    DisplayServer::singleton().finalize();
    UIManager::finalize();
    SceneManager::singleton().finalize();

    asset_loader::finalize();
    jobsystem::finalize();
    thread::finailize();

    DynamicVariableManager::serialize();

    OS::singleton().finalize();

    return 0;
}

}  // namespace vct