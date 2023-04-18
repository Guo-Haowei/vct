#include "WindowManager.h"

#include <GLFW/glfw3.h>

#include "CommonDvars.h"
#include "imgui/imgui.h"
#include "Core/Check.h"
#include "Core/Log.h"

WindowManager* gWindowManager = new WindowManager();

bool WindowManager::InitializeInternal()
{
    check(!mInitialized);

    glfwSetErrorCallback([](int code, const char* desc) {
        LOG_FATAL("[glfw] error({}): {}", code, desc);
    });

    glfwInit();

    glfwWindowHint(GLFW_DECORATED, !DVAR_GET_BOOL(wnd_frameless));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    if (DVAR_GET_BOOL(r_debug))
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    }

    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    const ivec2 maxSize = ivec2(vidmode->width, vidmode->height);
    ivec2 size(DVAR_GET_INT(wnd_width), DVAR_GET_INT(wnd_height));
    if (size.x == 0 || size.y == 0)
    {
        size.x = int(0.8f * maxSize.x);
        size.y = int(0.8f * maxSize.y);
    }

    constexpr ivec2 MIN_FRAME_SIZE = ivec2(800, 600);

    size = glm::clamp(size, MIN_FRAME_SIZE, maxSize);

    mWindow = glfwCreateWindow(int(size.x), int(size.y), "Editor", 0, 0);
    glfwMakeContextCurrent(mWindow);

    LOG_OK("MainWindow created {} x {}", size.x, size.y);
    glfwGetFramebufferSize(mWindow, &mFrameSize.x, &mFrameSize.y);
    return mInitialized = true;
}

void WindowManager::FinalizeInternal()
{
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
             "Editor",
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
