#include "Application.h"

#include "imgui/imgui.h"
// @TODO: refactor

#include "core/dynamic_variable/common_dvars.h"
#include "core/input/input.h"
#include "core/systems/job_system.h"
#include "core/utility/timer.h"
#include "scene/scene_manager.h"
#include "servers/display_server.h"
#include "servers/rendering_server.h"

using namespace vct;

void Application::AddLayer(std::shared_ptr<Layer> layer) { mLayers.emplace_back(layer); }

int Application::Run(int, const char**) {
    for (auto& layer : mLayers) {
        layer->Attach();
        LOG("[Runtime] layer '{}' attached!", layer->GetName());
    }

    LOG("\n********************************************************************************"
        "\nMain Loop"
        "\n********************************************************************************");

    LOG_WARN("TODO: path tracer here");
    LOG_ERROR("TODO: migrate PBR code here");
    LOG_WARN("TODO: fix multiple objects play same animation");
    LOG_ERROR("TODO: link object with animation");
    LOG_WARN("TODO: save and load scene");
    LOG_ERROR("TODO: cloth physics");
    LOG_WARN("TODO: refactor application");

    // @TODO: add frame count, elapsed time, etc
    Timer timer;
    while (!DisplayServer::singleton().should_close()) {
        DisplayServer::singleton().new_frame();

        input::begin_frame();

        // @TODO: better elapsed time
        const float dt = static_cast<float>(timer.get_duration().to_second());
        SceneManager::singleton().update(dt);
        timer.start();

        ImGui::NewFrame();
        for (auto& layer : mLayers) {
            layer->Update(dt);
        }

        for (auto& layer : mLayers) {
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

    return 0;
}
