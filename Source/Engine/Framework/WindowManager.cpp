#include "WindowManager.h"

#include "Application.h"
#include "Core/CommonDvars.h"
#include "Core/Check.h"
#include "Core/Input.h"
#include "Core/Log.h"

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
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

    ImGui_ImplGlfw_InitForOpenGL(mWindow, false);

    // glfwSetFramebufferSizeCallback(mGlfwWindow, [](GLFWwindow* window, int width, int height) {
    //     auto data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
    //     data->mWidth = width;
    //     data->mHeight = height;
    //     Event e = Event::Resize(width, height);
    //     data->mFunc(e);
    // });

    glfwSetCursorPosCallback(mWindow, CursorPosCallback);
    glfwSetMouseButtonCallback(mWindow, MouseButtonCallback);
    glfwSetKeyCallback(mWindow, KeyCallback);
    glfwSetScrollCallback(mWindow, ScrollCallback);

    glfwSetWindowFocusCallback(mWindow, ImGui_ImplGlfw_WindowFocusCallback);
    glfwSetCursorEnterCallback(mWindow, ImGui_ImplGlfw_CursorEnterCallback);
    glfwSetCharCallback(mWindow, ImGui_ImplGlfw_CharCallback);

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

    ImGui_ImplGlfw_NewFrame();
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
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    GLFWwindow* oldContext = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(oldContext);

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

void WindowManager::CursorPosCallback(GLFWwindow* window, double x, double y)
{
    ImGui_ImplGlfw_CursorPosCallback(window, x, y);
    // if (!ImGui::GetIO().WantCaptureMouse)
    {
        Input::gInput.SetCursor(static_cast<float>(x), static_cast<float>(y));
    }
}

void WindowManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    // if (!ImGui::GetIO().WantCaptureMouse)
    {
        if (action == GLFW_PRESS)
        {
            Input::gInput.SetButton(button, true);
        }
        else if (action == GLFW_RELEASE)
        {
            Input::gInput.SetButton(button, false);
        }
    }
}

void WindowManager::KeyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, keycode, scancode, action, mods);

    // if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        if (action == GLFW_PRESS)
        {
            Input::gInput.SetKey(keycode, true);
        }
        else if (action == GLFW_RELEASE)
        {
            Input::gInput.SetKey(keycode, false);
        }
    }
}

void WindowManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    // if (!ImGui::GetIO().WantCaptureMouse)
    {
        Input::gInput.SetWheel(static_cast<float>(xoffset), static_cast<float>(yoffset));
    }
}

#include "imgui/backends/imgui_impl_glfw.cpp"