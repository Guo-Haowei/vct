#include "application.h"

#include "imgui/imgui.h"
// @TODO: refactor

#include "assets/asset_loader.h"
#include "core/dynamic_variable/dynamic_variable_manager.h"
#include "core/framework/common_dvars.h"
#include "core/framework/graphics_manager.h"
#include "core/framework/imgui_module.h"
#include "core/framework/physics_manager.h"
#include "core/framework/scene_manager.h"
#include "core/input/input.h"
#include "core/os/threads.h"
#include "core/os/timer.h"
#include "core/systems/job_system.h"
#include "rendering/rendering_dvars.h"
#include "rendering/rendering_misc.h"
#include "rendering/shader_program_manager.h"
#include "servers/display_server.h"
#include "servers/display_server_glfw.h"

#define DEFINE_DVAR
#include "core/framework/common_dvars.h"

namespace vct {

static void register_common_dvars() {
#define REGISTER_DVAR
#include "core/framework/common_dvars.h"
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

void Application::register_module(Module* module) {
    module->m_app = this;
    m_modules.push_back(module);
}

void Application::setup_modules() {
    m_scene_manager = std::make_shared<SceneManager>();
    m_physics_manager = std::make_shared<PhysicsManager>();
    m_imgui_module = std::make_shared<ImGuiModule>();
    m_display_server = std::make_shared<DisplayServerGLFW>();
    m_graphics_manager = std::make_shared<GraphicsManager>();

    register_module(m_scene_manager.get());
    register_module(m_physics_manager.get());
    register_module(m_imgui_module.get());
    register_module(m_display_server.get());
    register_module(m_graphics_manager.get());

    m_event_queue.register_listener(m_graphics_manager.get());
    m_event_queue.register_listener(m_physics_manager.get());
}

int Application::run(int argc, const char** argv) {
    save_command_line(argc, argv);
    m_os = std::make_shared<OS>();
    setup_modules();

    // intialize
    OS::singleton().initialize();

    // dvars
    register_common_dvars();
    register_rendering_dvars();
    DynamicVariableManager::deserialize();
    DynamicVariableManager::parse(m_command_line);

    thread::initialize();
    jobsystem::initialize();
    asset_loader::initialize();

    for (Module* module : m_modules) {
        LOG("module '{}' being initialized...", module->get_name());
        if (!module->initialize()) {
            LOG_ERROR("Error: failed to initialize module '{}'", module->get_name());
            return 1;
        }
        LOG("module '{}' initialized\n", module->get_name());
    }

    ShaderProgramManager::singleton().initialize();

    init_layers();
    for (auto& layer : m_layers) {
        layer->attach();
        LOG("[Runtime] layer '{}' attached!", layer->get_name());
    }

    LOG("\n********************************************************************************"
        "\nMain Loop"
        "\n********************************************************************************");

    LOG_WARN("TODO: editor add/remove component");
    LOG_ERROR("TODO: cloth physics");
    LOG_WARN("TODO: better camera controller (both orbit and 3rd person)");
    LOG_ERROR("TODO: FXAA");
    LOG_WARN("TODO: TAA");
    LOG_ERROR("TODO: soft shadow");
    LOG_WARN("TODO: path tracer here");
    LOG_ERROR("TODO: migrate PBR code here");
    LOG_WARN("TODO: fix multiple objects play same animation");
    LOG_ERROR("TODO: link object with animation");
    LOG_WARN("TODO: render texels around camera");

    // @TODO: add frame count, elapsed time, etc
    Timer timer;
    while (!DisplayServer::singleton().should_close()) {
        m_display_server->new_frame();

        input::begin_frame();

        // @TODO: better elapsed time
        float dt = static_cast<float>(timer.get_duration().to_second());
        dt = glm::min(dt, 0.1f);
        m_scene_manager->update(dt);
        timer.start();

        ImGui::NewFrame();
        for (auto& layer : m_layers) {
            layer->update(dt);
        }

        for (auto& layer : m_layers) {
            layer->render();
        }
        ImGui::Render();

        m_physics_manager->update(dt);

        m_graphics_manager->render();

        m_display_server->present();

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

    for (int index = (int)m_modules.size() - 1; index >= 0; --index) {
        Module* module = m_modules[index];
        module->finalize();
        LOG_VERBOSE("module '{}' finalized", module->get_name());
    }

    asset_loader::finalize();
    jobsystem::finalize();
    thread::finailize();

    DynamicVariableManager::serialize();

    OS::singleton().finalize();

    return 0;
}

}  // namespace vct