#include "WindowManager.h"

#include "Application.h"
#include "Core/CommonDvars.h"
#include "Core/Check.h"
#include "Core/Log.h"

#include "imgui/backends/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"

WindowManager* gWindowManager = new WindowManager();

bool WindowManager::InitializeInternal()
{
    const auto& info = mApplication->GetInfo();

    check(!mInitialized);

    glfwSetErrorCallback([](int code, const char* desc) {
        LOG_FATAL("[glfw] error({}): {}", code, desc);
    });

    glfwInit();

    glfwWindowHint(GLFW_DECORATED, !info.frameless);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    if (DVAR_GET_BOOL(r_debug))
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    }

    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const ivec2 minSize = ivec2(600, 400);
    const ivec2 maxSize = ivec2(vidmode->width, vidmode->height);
    const ivec2 size = glm::clamp(ivec2(info.width, info.height), minSize, maxSize);

    mWindow = glfwCreateWindow(int(size.x), int(size.y), info.title, 0, 0);
    check(mWindow);

    glfwMakeContextCurrent(mWindow);

    LOG_OK("GLFWwindow created {} x {}", size.x, size.y);
    glfwGetFramebufferSize(mWindow, &mFrameSize.x, &mFrameSize.y);

    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    return true;
}

void WindowManager::FinalizeInternal()
{
    ImGui_ImplGlfw_Shutdown();
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

bool WindowManager::ShouldClose()
{
    return glfwWindowShouldClose(mWindow);
}

GLFWwindow* WindowManager::GetRaw()
{
    check(mInitialized && mWindow);
    return mWindow;
}

void WindowManager::NewFrame()
{
    glfwPollEvents();
    glfwGetFramebufferSize(mWindow, &mFrameSize.x, &mFrameSize.y);

    // mouse position
    {
        double x, y;
        glfwGetCursorPos(mWindow, &x, &y);
        mMousePos.x = static_cast<float>(x);
        mMousePos.y = static_cast<float>(y);
    }

    // title
    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
             "%s | Size: %d x %d | Mouse: %d x %d | FPS: %.1f",
             mApplication->GetInfo().title,
             mFrameSize.x, mFrameSize.y,
             int(mMousePos.x), int(mMousePos.y),
             ImGui::GetIO().Framerate);
    glfwSetWindowTitle(mWindow, buffer);
}

std::tuple<int, int> WindowManager::GetFrameSize()
{
    return std::tuple<int, int>(mFrameSize.x, mFrameSize.y);
}

std::tuple<float, float> WindowManager::GetMousePos()
{
    return std::tuple<float, float>(mMousePos.x, mMousePos.y);
}

void WindowManager::Present()
{
    glfwSwapBuffers(mWindow);
}

bool WindowManager::IsKeyDown(int code)
{
    return ImGui::IsKeyDown((ImGuiKey)code);
}

bool WindowManager::IsMouseInScreen()
{
    bool inside = true;
    inside &= mMousePos.x >= 0;
    inside &= mMousePos.y >= 0;
    inside &= mMousePos.x <= mFrameSize.x;
    inside &= mMousePos.y <= mFrameSize.y;
    return inside;
}
