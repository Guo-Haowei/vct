#include "display_server_glfw.h"

#include "Framework/Application.h"
#include "GLFW/glfw3.h"
#include "core/Input.h"
#include "core/dynamic_variable/common_dvars.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

namespace vct {

bool DisplayServerGLFW::initialize() {
    glfwSetErrorCallback([](int code, const char* desc) { LOG_FATAL("[glfw] error({}): {}", code, desc); });

    glfwInit();

    bool frameless = false;
    glfwWindowHint(GLFW_DECORATED, !frameless);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    if (DVAR_GET_BOOL(r_gpu_validation)) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    }

    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    const ivec2 resolution = DVAR_GET_IVEC2(window_resolution);
    const ivec2 minSize = ivec2(600, 400);
    const ivec2 maxSize = ivec2(vidmode->width, vidmode->height);
    const ivec2 size = glm::clamp(resolution, minSize, maxSize);

    m_window = glfwCreateWindow(size.x, size.y, "Editor", nullptr, nullptr);
    DEV_ASSERT(m_window);

    const ivec2 position = DVAR_GET_IVEC2(window_position);
    glfwSetWindowPos(m_window, position.x, position.y);

    glfwMakeContextCurrent(m_window);

    LOG("GLFWwindow created {} x {}", size.x, size.y);
    glfwGetFramebufferSize(m_window, &m_frame_size.x, &m_frame_size.y);

    ImGui_ImplGlfw_InitForOpenGL(m_window, false);

    // glfwSetFramebufferSizeCallback(mGlfwWindow, [](GLFWwindow* window, int width, int height) {
    //     auto data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
    //     data->mWidth = width;
    //     data->mHeight = height;
    //     Event e = Event::Resize(width, height);
    //     data->mFunc(e);
    // });

    glfwSetCursorPosCallback(m_window, cursor_pos_cb);
    glfwSetMouseButtonCallback(m_window, mouse_button_cb);
    glfwSetKeyCallback(m_window, key_cb);
    glfwSetScrollCallback(m_window, scroll_cb);

    glfwSetWindowFocusCallback(m_window, ImGui_ImplGlfw_WindowFocusCallback);
    glfwSetCursorEnterCallback(m_window, ImGui_ImplGlfw_CursorEnterCallback);
    glfwSetCharCallback(m_window, ImGui_ImplGlfw_CharCallback);

    return true;
}

void DisplayServerGLFW::finalize() {
    ImGui_ImplGlfw_Shutdown();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool DisplayServerGLFW::should_close() {
    return glfwWindowShouldClose(m_window);
}

void DisplayServerGLFW::new_frame() {
    glfwPollEvents();
    glfwGetFramebufferSize(m_window, &m_frame_size.x, &m_frame_size.y);
    glfwGetWindowPos(m_window, &m_window_pos.x, &m_window_pos.y);

    // title
    auto title = std::format("{} | Pos: {}x{} | Size: {}x{} | FPS: {:.1f}",
                             "Editor",
                             m_window_pos.x,
                             m_window_pos.y,
                             m_frame_size.x,
                             m_frame_size.y,
                             ImGui::GetIO().Framerate);
    glfwSetWindowTitle(m_window, title.c_str());

    ImGui_ImplGlfw_NewFrame();
}

std::tuple<int, int> DisplayServerGLFW::get_frame_size() { return std::tuple<int, int>(m_frame_size.x, m_frame_size.y); }

std::tuple<int, int> DisplayServerGLFW::get_window_pos() { return std::tuple<int, int>(m_window_pos.x, m_window_pos.y); }

void DisplayServerGLFW::present() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    GLFWwindow* oldContext = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(oldContext);

    glfwSwapBuffers(m_window);
}

void DisplayServerGLFW::cursor_pos_cb(GLFWwindow* window, double x, double y) {
    ImGui_ImplGlfw_CursorPosCallback(window, x, y);
    // if (!ImGui::GetIO().WantCaptureMouse)
    { Input::gInput.SetCursor(static_cast<float>(x), static_cast<float>(y)); }
}

void DisplayServerGLFW::mouse_button_cb(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    // if (!ImGui::GetIO().WantCaptureMouse)
    {
        if (action == GLFW_PRESS) {
            Input::gInput.SetButton(button, true);
        } else if (action == GLFW_RELEASE) {
            Input::gInput.SetButton(button, false);
        }
    }
}

void DisplayServerGLFW::key_cb(GLFWwindow* window, int keycode, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, keycode, scancode, action, mods);

    // if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        if (action == GLFW_PRESS) {
            Input::gInput.SetKey(keycode, true);
        } else if (action == GLFW_RELEASE) {
            Input::gInput.SetKey(keycode, false);
        }
    }
}

void DisplayServerGLFW::scroll_cb(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    // if (!ImGui::GetIO().WantCaptureMouse)
    { Input::gInput.SetWheel(static_cast<float>(xoffset), static_cast<float>(yoffset)); }
}

}  // namespace vct

#include "imgui/backends/imgui_impl_glfw.cpp"
