#include "Application.h"

#include "Core/Input.h"
#include "Core/JobSystem.h"
#include "Graphics/MainRenderer.h"
#include "GraphicsManager.h"
#include "ProgramManager.h"
#include "SceneManager.h"
#include "imgui/imgui.h"
// @TODO: refactor

#include "core/dynamic_variable/common_dvars.h"
#include "servers/display_server.h"

class JobSystemManager : public ManagerBase {
public:
    JobSystemManager() : ManagerBase("JobSystemManager") {}

protected:
    virtual bool InitializeInternal() override { return jobsystem::initialize(); }

    virtual void FinalizeInternal() override { jobsystem::finalize(); }
};

static JobSystemManager gJobSystemManager;

void Application::RegisterManager(ManagerBase* manager) {
    mManagers.emplace_back(manager);
    manager->mApplication = this;
}

bool Application::RegisterManagers() {
    RegisterManager(&gJobSystemManager);
    RegisterManager(gGraphicsManager);
    RegisterManager(gProgramManager);
    RegisterManager(gSceneManager);
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
    while (!DisplayServerGLFW::singleton().should_close()) {
        DisplayServerGLFW::singleton().new_frame();

        Input::BeginFrame();

        gSceneManager->Update(dt);

        ImGui::NewFrame();
        for (auto& layer : mLayers) {
            layer->Update(dt);
        }

        for (auto& layer : mLayers) {
            layer->Render();
        }
        ImGui::Render();

        renderer.render();

        DisplayServerGLFW::singleton().present();

        ImGui::EndFrame();
    }

    auto [w, h] = DisplayServerGLFW::singleton().get_frame_size();
    DVAR_SET_IVEC2(window_resolution, w, h);
    auto [x, y] = DisplayServerGLFW::singleton().get_window_pos();
    DVAR_SET_IVEC2(window_position, x, y);

    renderer.destroyGpuResources();

    FinalizeManagers();

    return 0;
}
