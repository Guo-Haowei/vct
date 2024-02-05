#include "Application.h"

#include "Core/CommonDvars.h"
#include "Core/Input.h"
#include "Core/JobSystem.h"
#include "GraphicsManager.h"
#include "ProgramManager.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "WindowManager.h"

// @TODO: refactor
#include "Core/lua_script.h"
#include "Graphics/MainRenderer.h"
#include "imgui/imgui.h"

#define DEFINE_DVAR
#include "Core/CommonDvars.h"

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

int Application::Run(int argc, const char** argv) {
    for (int i = 1; i < argc; ++i) {
        mCommandLine.emplace_back(std::string(argv[i]));
    }

    bool ok = true;
    ok = ok && ProcessCmdLine();
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

    renderer.destroyGpuResources();

    FinalizeManagers();

    return 0;
}

static void register_common_dvars() {
#define REGISTER_DVAR
#include "Core/CommonDvars.h"
}

class CommandHelper {
public:
    CommandHelper(const CommandLine& cmdLine) : mCommandLine(cmdLine) {}

    bool TryConsume(std::string& str) {
        if (mCommandLine.empty()) {
            str.clear();
            return false;
        }

        str = mCommandLine.front();
        mCommandLine.pop_front();
        return true;
    }

    bool Consume(std::string& str) {
        if (mCommandLine.empty()) {
            LOG_ERROR("Unexpected EOF");
            str.clear();
            return false;
        }

        return TryConsume(str);
    }

private:
    CommandLine mCommandLine;
};

bool Application::ProcessCmdLine() {
    register_common_dvars();

    CommandHelper cmdHelper(mCommandLine);

    std::string arg;
    while (cmdHelper.TryConsume(arg)) {
        if (arg == "+set") {
            cmdHelper.Consume(arg);
            DynamicVariable* dvar = DynamicVariable::find_dvar(arg.c_str());
            if (dvar == nullptr) {
                LOG_ERROR("[dvar] Dvar '{}' not found", arg);
                return false;
            }
            cmdHelper.Consume(arg);
            dvar->set_from_source_string(arg.c_str());
        } else if (arg == "+exec") {
            cmdHelper.Consume(arg);
            if (!Com_ExecLua(arg.c_str())) {
                LOG_ERROR("[lua] failed to execute script '{}'", arg);
                return false;
            }
        } else {
            LOG_ERROR("Unknown command '{}'", arg);
            return false;
        }
    }

    return true;
}
