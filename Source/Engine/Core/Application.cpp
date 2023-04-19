#include "Application.h"

#include "CommonDvars.h"
#include "Log.h"
#include "UIManager.h"
#include "WindowManager.h"
#include "Graphics/GraphicsManager.h"

// @TODO: refactor
#include "lua_script.h"
#include "com_misc.h"
#include "Graphics/MainRenderer.h"
#include "editor.h"

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

bool Application::RegisterManagers()
{
    mManagers.push_back(gUIManager);
    mManagers.push_back(gWindowManager);
    mManagers.push_back(gGraphicsManager);
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

bool Application::Run(int argc, const char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        mCommandLine.push_back(argv[i]);
    }

    bool ok = true;
    ok = ok && ProcessCmdLine();
    ok = ok && RegisterManagers();
    ok = ok && InitializeManagers();

    ok = ok && Com_LoadScene();

    if (!ok)
    {
        return false;
    }

    vct::MainRenderer renderer;
    renderer.createGpuResources();

    while (!gWindowManager->ShouldClose())
    {
        gWindowManager->NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        EditorSetup();
        ImGui::Render();

        Com_UpdateWorld();

        renderer.render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);

        gWindowManager->Present();

        Com_GetScene().dirty = false;
    }

    renderer.destroyGpuResources();

    FinalizeManagers();

    return true;
}

static void register_common_dvars()
{
#define REGISTER_DVAR
#include "CommonDvars.h"
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
