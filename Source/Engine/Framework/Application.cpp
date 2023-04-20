#include "Application.h"

#include "GraphicsManager.h"
#include "WindowManager.h"
#include "SceneManager.h"
#include "ProgramManager.h"
#include "UIManager.h"
#include "Core/CommonDvars.h"
#include "Core/Input.h"
#include "Core/Log.h"

// @TODO: refactor
#include "Core/lua_script.h"
#include "Graphics/MainRenderer.h"
#include "imgui/imgui.h"

#define DEFINE_DVAR
#include "Core/CommonDvars.h"

void Application::RegisterManager(ManagerBase* manager)
{
    mManagers.emplace_back(manager);
    manager->mApplication = this;
}

bool Application::RegisterManagers()
{
    RegisterManager(gUIManager);
    RegisterManager(gWindowManager);
    RegisterManager(gGraphicsManager);
    RegisterManager(gProgramManager);
    RegisterManager(gSceneManager);
    return true;
}

bool Application::InitializeManagers()
{
    for (auto manager : mManagers)
    {
        if (!manager->Initialize())
        {
            return false;
        }
    }

    return true;
}

void Application::FinalizeManagers()
{
    for (auto it = mManagers.rbegin(); it != mManagers.rend(); ++it)
    {
        (*it)->Finalize();
    }
}

void Application::AddLayer(std::shared_ptr<Layer> layer)
{
    mLayers.emplace_back(layer);
}

int Application::Run(int argc, const char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        mCommandLine.emplace_back(std::string(argv[i]));
    }

    bool ok = true;
    ok = ok && ProcessCmdLine();
    ok = ok && RegisterManagers();
    ok = ok && InitializeManagers();

    if (!ok)
    {
        return -1;
    }

    vct::MainRenderer renderer;
    renderer.createGpuResources();

    for (auto& layer : mLayers)
    {
        layer->Attach();
        LOG_OK("[Runtime] layer '{}' attached!", layer->GetName());
    }

    float dt = 0.0f;
    while (!gWindowManager->ShouldClose())
    {
        gWindowManager->NewFrame();

        Input::BeginFrame();

        ImGui::NewFrame();
        for (auto& layer : mLayers)
        {
            layer->Update(dt);
        }

        for (auto& layer : mLayers)
        {
            layer->Render();
        }
        ImGui::Render();

        gSceneManager->Update(dt);

        renderer.render();

        gWindowManager->Present();

        Com_GetScene().dirty = false;

        Input::EndFrame();
    }

    renderer.destroyGpuResources();

    FinalizeManagers();

    return 0;
}

static void register_common_dvars()
{
#define REGISTER_DVAR
#include "Core/CommonDvars.h"
}

class CommandHelper
{
public:
    CommandHelper(const CommandLine& cmdLine) : mCommandLine(cmdLine)
    {
    }

    bool TryConsume(std::string& str)
    {
        if (mCommandLine.empty())
        {
            str.clear();
            return false;
        }

        str = mCommandLine.front();
        mCommandLine.pop_front();
        return true;
    }

    bool Consume(std::string& str)
    {
        if (mCommandLine.empty())
        {
            LOG_ERROR("Unexpected EOF");
            str.clear();
            return false;
        }

        return TryConsume(str);
    }

private:
    CommandLine mCommandLine;
};

bool Application::ProcessCmdLine()
{
    register_common_dvars();

    CommandHelper cmdHelper(mCommandLine);

    std::string arg;
    while (cmdHelper.TryConsume(arg))
    {
        if (arg == "+set")
        {
            cmdHelper.Consume(arg);
            DynamicVariable* dvar = DynamicVariableManager::Find(arg.c_str());
            if (dvar == nullptr)
            {
                LOG_ERROR("[dvar] Dvar '{}' not found", arg);
                return false;
            }
            cmdHelper.Consume(arg);
            dvar->SetFromSourceString(arg.c_str());
        }
        else if (arg == "+exec")
        {
            cmdHelper.Consume(arg);
            if (!Com_ExecLua(arg.c_str()))
            {
                LOG_ERROR("[lua] failed to execute script '{}'", arg);
                return false;
            }
        }
        else
        {
            LOG_ERROR("Unknown command '{}'", arg);
            return false;
        }
    }

    return true;
}
