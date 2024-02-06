#include "Application.h"

#include "Core/Input.h"
#include "GraphicsManager.h"
#include "ProgramManager.h"
#include "SceneManager.h"
#include "imgui/imgui.h"
#include "servers/rendering/MainRenderer.h"
// @TODO: refactor

#include "core/dynamic_variable/common_dvars.h"
#include "core/systems/job_system.h"
#include "servers/display_server.h"

using namespace vct;

void Application::RegisterManager(ManagerBase* manager) {
    mManagers.emplace_back(manager);
    manager->mApplication = this;
}

bool Application::RegisterManagers() {
    RegisterManager(gGraphicsManager);
    RegisterManager(gProgramManager);
    return true;
}

bool Application::InitializeManagers() {
    for (auto manager : mManagers) {
        if (!manager->Initialize()) {
            return false;
        }
    }

    return true;
}

void Application::FinalizeManagers() {
    for (auto it = mManagers.rbegin(); it != mManagers.rend(); ++it) {
        (*it)->Finalize();
    }
}

void Application::AddLayer(std::shared_ptr<Layer> layer) { mLayers.emplace_back(layer); }

int Application::Run(int, const char**) {
    bool ok = true;
    ok = ok && RegisterManagers();
    ok = ok && InitializeManagers();

    SceneManager::singleton().initialize();

    if (!ok) {
        return -1;
    }

    vct::MainRenderer renderer;
    renderer.createGpuResources();

    for (auto& layer : mLayers) {
        layer->Attach();
        LOG("[Runtime] layer '{}' attached!", layer->GetName());
    }

    float dt = 0.0f;
    while (!DisplayServer::singleton().should_close()) {
        DisplayServer::singleton().new_frame();

        Input::BeginFrame();

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

        renderer.render();

        DisplayServer::singleton().present();

        ImGui::EndFrame();
    }

    auto [w, h] = DisplayServer::singleton().get_frame_size();
    DVAR_SET_IVEC2(window_resolution, w, h);
    auto [x, y] = DisplayServer::singleton().get_window_pos();
    DVAR_SET_IVEC2(window_position, x, y);

    renderer.destroyGpuResources();

    SceneManager::singleton().finalize();

    FinalizeManagers();

    return 0;
}
