#include "display_server_glfw.h"

#include <GLFW/glfw3.h>

#include "core/framework/application.h"
#include "core/framework/common_dvars.h"
#include "core/input/input.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "rendering/rendering_dvars.h"

namespace vct {

bool DisplayServerGLFW::initialize() {
    DisplayServerGLFW::initialize_key_mapping();

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
    //     data->m_func(e);
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
    { input::set_cursor(static_cast<float>(x), static_cast<float>(y)); }
}

void DisplayServerGLFW::mouse_button_cb(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    // if (!ImGui::GetIO().WantCaptureMouse)
    {
        if (action == GLFW_PRESS) {
            input::set_button(button, true);
        } else if (action == GLFW_RELEASE) {
            input::set_button(button, false);
        }
    }
}

void DisplayServerGLFW::key_cb(GLFWwindow* window, int keycode, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, keycode, scancode, action, mods);

    // if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        DEV_ASSERT(s_key_mapping.find(keycode) != s_key_mapping.end());
        KeyCode key = s_key_mapping[keycode];

        if (action == GLFW_PRESS) {
            input::set_key(key, true);
        } else if (action == GLFW_RELEASE) {
            input::set_key(key, false);
        }
    }
}

void DisplayServerGLFW::scroll_cb(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    // if (!ImGui::GetIO().WantCaptureMouse)
    { input::set_wheel(static_cast<float>(xoffset), static_cast<float>(yoffset)); }
}

void DisplayServerGLFW::initialize_key_mapping() {
    if (!s_key_mapping.empty()) {
        return;
    }

    s_key_mapping[GLFW_KEY_SPACE] = KEY_SPACE;
    s_key_mapping[GLFW_KEY_APOSTROPHE] = KEY_APOSTROPHE;
    s_key_mapping[GLFW_KEY_COMMA] = KEY_COMMA;
    s_key_mapping[GLFW_KEY_MINUS] = KEY_MINUS;
    s_key_mapping[GLFW_KEY_PERIOD] = KEY_PERIOD;
    s_key_mapping[GLFW_KEY_SLASH] = KEY_SLASH;
    s_key_mapping[GLFW_KEY_0] = KEY_0;
    s_key_mapping[GLFW_KEY_1] = KEY_1;
    s_key_mapping[GLFW_KEY_2] = KEY_2;
    s_key_mapping[GLFW_KEY_3] = KEY_3;
    s_key_mapping[GLFW_KEY_4] = KEY_4;
    s_key_mapping[GLFW_KEY_5] = KEY_5;
    s_key_mapping[GLFW_KEY_6] = KEY_6;
    s_key_mapping[GLFW_KEY_7] = KEY_7;
    s_key_mapping[GLFW_KEY_8] = KEY_8;
    s_key_mapping[GLFW_KEY_9] = KEY_9;
    s_key_mapping[GLFW_KEY_SEMICOLON] = KEY_SEMICOLON;
    s_key_mapping[GLFW_KEY_EQUAL] = KEY_EQUAL;
    s_key_mapping[GLFW_KEY_A] = KEY_A;
    s_key_mapping[GLFW_KEY_B] = KEY_B;
    s_key_mapping[GLFW_KEY_C] = KEY_C;
    s_key_mapping[GLFW_KEY_D] = KEY_D;
    s_key_mapping[GLFW_KEY_E] = KEY_E;
    s_key_mapping[GLFW_KEY_F] = KEY_F;
    s_key_mapping[GLFW_KEY_G] = KEY_G;
    s_key_mapping[GLFW_KEY_H] = KEY_H;
    s_key_mapping[GLFW_KEY_I] = KEY_I;
    s_key_mapping[GLFW_KEY_J] = KEY_J;
    s_key_mapping[GLFW_KEY_K] = KEY_K;
    s_key_mapping[GLFW_KEY_L] = KEY_L;
    s_key_mapping[GLFW_KEY_M] = KEY_M;
    s_key_mapping[GLFW_KEY_N] = KEY_N;
    s_key_mapping[GLFW_KEY_O] = KEY_O;
    s_key_mapping[GLFW_KEY_P] = KEY_P;
    s_key_mapping[GLFW_KEY_Q] = KEY_Q;
    s_key_mapping[GLFW_KEY_R] = KEY_R;
    s_key_mapping[GLFW_KEY_S] = KEY_S;
    s_key_mapping[GLFW_KEY_T] = KEY_T;
    s_key_mapping[GLFW_KEY_U] = KEY_U;
    s_key_mapping[GLFW_KEY_V] = KEY_V;
    s_key_mapping[GLFW_KEY_W] = KEY_W;
    s_key_mapping[GLFW_KEY_X] = KEY_X;
    s_key_mapping[GLFW_KEY_Y] = KEY_Y;
    s_key_mapping[GLFW_KEY_Z] = KEY_Z;

    s_key_mapping[GLFW_KEY_LEFT_BRACKET] = KEY_LEFT_BRACKET;
    s_key_mapping[GLFW_KEY_BACKSLASH] = KEY_BACKSLASH;
    s_key_mapping[GLFW_KEY_RIGHT_BRACKET] = KEY_RIGHT_BRACKET;
    s_key_mapping[GLFW_KEY_GRAVE_ACCENT] = KEY_GRAVE_ACCENT;
    s_key_mapping[GLFW_KEY_WORLD_1] = KEY_WORLD_1;
    s_key_mapping[GLFW_KEY_WORLD_2] = KEY_WORLD_2;
    s_key_mapping[GLFW_KEY_ESCAPE] = KEY_ESCAPE;
    s_key_mapping[GLFW_KEY_ENTER] = KEY_ENTER;
    s_key_mapping[GLFW_KEY_TAB] = KEY_TAB;
    s_key_mapping[GLFW_KEY_BACKSPACE] = KEY_BACKSPACE;
    s_key_mapping[GLFW_KEY_INSERT] = KEY_INSERT;
    s_key_mapping[GLFW_KEY_DELETE] = KEY_DELETE;
    s_key_mapping[GLFW_KEY_RIGHT] = KEY_RIGHT;
    s_key_mapping[GLFW_KEY_LEFT] = KEY_LEFT;
    s_key_mapping[GLFW_KEY_DOWN] = KEY_DOWN;
    s_key_mapping[GLFW_KEY_UP] = KEY_UP;
    s_key_mapping[GLFW_KEY_PAGE_UP] = KEY_PAGE_UP;
    s_key_mapping[GLFW_KEY_PAGE_DOWN] = KEY_PAGE_DOWN;
    s_key_mapping[GLFW_KEY_HOME] = KEY_HOME;
    s_key_mapping[GLFW_KEY_END] = KEY_END;
    s_key_mapping[GLFW_KEY_CAPS_LOCK] = KEY_CAPS_LOCK;
    s_key_mapping[GLFW_KEY_SCROLL_LOCK] = KEY_SCROLL_LOCK;
    s_key_mapping[GLFW_KEY_NUM_LOCK] = KEY_NUM_LOCK;
    s_key_mapping[GLFW_KEY_PRINT_SCREEN] = KEY_PRINT_SCREEN;
    s_key_mapping[GLFW_KEY_PAUSE] = KEY_PAUSE;
    s_key_mapping[GLFW_KEY_F1] = KEY_F1;
    s_key_mapping[GLFW_KEY_F2] = KEY_F2;
    s_key_mapping[GLFW_KEY_F3] = KEY_F3;
    s_key_mapping[GLFW_KEY_F4] = KEY_F4;
    s_key_mapping[GLFW_KEY_F5] = KEY_F5;
    s_key_mapping[GLFW_KEY_F6] = KEY_F6;
    s_key_mapping[GLFW_KEY_F7] = KEY_F7;
    s_key_mapping[GLFW_KEY_F8] = KEY_F8;
    s_key_mapping[GLFW_KEY_F9] = KEY_F9;
    s_key_mapping[GLFW_KEY_F10] = KEY_F10;
    s_key_mapping[GLFW_KEY_F11] = KEY_F11;
    s_key_mapping[GLFW_KEY_F12] = KEY_F12;
    s_key_mapping[GLFW_KEY_F13] = KEY_F13;
    s_key_mapping[GLFW_KEY_F14] = KEY_F14;
    s_key_mapping[GLFW_KEY_F15] = KEY_F15;
    s_key_mapping[GLFW_KEY_F16] = KEY_F16;
    s_key_mapping[GLFW_KEY_F17] = KEY_F17;
    s_key_mapping[GLFW_KEY_F18] = KEY_F18;
    s_key_mapping[GLFW_KEY_F19] = KEY_F19;
    s_key_mapping[GLFW_KEY_F20] = KEY_F20;
    s_key_mapping[GLFW_KEY_F21] = KEY_F21;
    s_key_mapping[GLFW_KEY_F22] = KEY_F22;
    s_key_mapping[GLFW_KEY_F23] = KEY_F23;
    s_key_mapping[GLFW_KEY_F24] = KEY_F24;
    s_key_mapping[GLFW_KEY_F25] = KEY_F25;
    s_key_mapping[GLFW_KEY_KP_0] = KEY_KP_0;
    s_key_mapping[GLFW_KEY_KP_1] = KEY_KP_1;
    s_key_mapping[GLFW_KEY_KP_2] = KEY_KP_2;
    s_key_mapping[GLFW_KEY_KP_3] = KEY_KP_3;
    s_key_mapping[GLFW_KEY_KP_4] = KEY_KP_4;
    s_key_mapping[GLFW_KEY_KP_5] = KEY_KP_5;
    s_key_mapping[GLFW_KEY_KP_6] = KEY_KP_6;
    s_key_mapping[GLFW_KEY_KP_7] = KEY_KP_7;
    s_key_mapping[GLFW_KEY_KP_8] = KEY_KP_8;
    s_key_mapping[GLFW_KEY_KP_9] = KEY_KP_9;
    s_key_mapping[GLFW_KEY_KP_DECIMAL] = KEY_KP_DECIMAL;
    s_key_mapping[GLFW_KEY_KP_DIVIDE] = KEY_KP_DIVIDE;
    s_key_mapping[GLFW_KEY_KP_MULTIPLY] = KEY_KP_MULTIPLY;
    s_key_mapping[GLFW_KEY_KP_SUBTRACT] = KEY_KP_SUBTRACT;
    s_key_mapping[GLFW_KEY_KP_ADD] = KEY_KP_ADD;
    s_key_mapping[GLFW_KEY_KP_ENTER] = KEY_KP_ENTER;
    s_key_mapping[GLFW_KEY_KP_EQUAL] = KEY_KP_EQUAL;
    s_key_mapping[GLFW_KEY_LEFT_SHIFT] = KEY_LEFT_SHIFT;
    s_key_mapping[GLFW_KEY_LEFT_CONTROL] = KEY_LEFT_CONTROL;
    s_key_mapping[GLFW_KEY_LEFT_ALT] = KEY_LEFT_ALT;
    s_key_mapping[GLFW_KEY_LEFT_SUPER] = KEY_LEFT_SUPER;
    s_key_mapping[GLFW_KEY_RIGHT_SHIFT] = KEY_RIGHT_SHIFT;
    s_key_mapping[GLFW_KEY_RIGHT_CONTROL] = KEY_RIGHT_CONTROL;
    s_key_mapping[GLFW_KEY_RIGHT_ALT] = KEY_RIGHT_ALT;
    s_key_mapping[GLFW_KEY_RIGHT_SUPER] = KEY_RIGHT_SUPER;
    s_key_mapping[GLFW_KEY_MENU] = KEY_MENU;
}

}  // namespace vct

#include "imgui/backends/imgui_impl_glfw.cpp"
