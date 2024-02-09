#include "Application.h"

#include "imgui/imgui.h"
// @TODO: refactor

#include "core/dynamic_variable/common_dvars.h"
#include "core/input/input.h"
#include "core/systems/job_system.h"
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

    float dt = 0.0f;
    while (!DisplayServer::singleton().should_close()) {
        DisplayServer::singleton().new_frame();

        input::begin_frame();

        // @TODO:
        SceneManager::singleton().update(dt);

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

    // @TODO: fix
    auto [w, h] = DisplayServer::singleton().get_frame_size();
    DVAR_SET_IVEC2(window_resolution, w, h);
    auto [x, y] = DisplayServer::singleton().get_window_pos();
    DVAR_SET_IVEC2(window_position, x, y);

    return 0;
}
