#include "Application.h"

#include "Core/Input.h"
#include "Core/JobSystem.h"
#include "GraphicsManager.h"
#include "ProgramManager.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "WindowManager.h"
#include "core/dynamic_variable/common_dvars.h"

// @TODO: refactor
#include "Core/lua_script.h"
#include "Graphics/MainRenderer.h"
#include "imgui/imgui.h"

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
    RegisterManager(gUIManager);
    RegisterManager(gWindowManager);
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
    while (!gWindowManager->ShouldClose()) {
        gWindowManager->NewFrame();

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

        gWindowManager->Present();

        ImGui::EndFrame();
    }

    auto [w, h] = gWindowManager->GetFrameSize();
    DVAR_SET_IVEC2(window_resolution, w, h);
    auto [x, y] = gWindowManager->GetWindowPos();
    DVAR_SET_IVEC2(window_position, x, y);

    renderer.destroyGpuResources();

    FinalizeManagers();

    return 0;
}
