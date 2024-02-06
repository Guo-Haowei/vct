#pragma once
#include <tuple>

#include "core/objects/singleton.h"

struct GLFWwindow;

namespace vct {

class DisplayServerGLFW : public Singleton<DisplayServerGLFW> {
public:
    bool should_close();
    GLFWwindow* get_window_ptr();

    std::tuple<int, int> get_frame_size();
    std::tuple<int, int> get_window_pos();

    void new_frame();
    void present();

    bool initialize();
    void finalize();

private:
    static void cursor_pos_cb(GLFWwindow* window, double x, double y);
    static void mouse_button_cb(GLFWwindow* window, int button, int action, int mods);
    static void key_cb(GLFWwindow* window, int keycode, int scancode, int action, int mods);
    static void scroll_cb(GLFWwindow* window, double xoffset, double yoffset);

    GLFWwindow* m_window = nullptr;
    struct {
        int x, y;
    } m_frame_size, m_window_pos;
};

}  // namespace vct
