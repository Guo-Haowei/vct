#pragma once
#include "display_server.h"

struct GLFWwindow;

namespace vct {

class DisplayServerGLFW : public DisplayServer {
public:
    virtual bool should_close() override;

    virtual std::tuple<int, int> get_frame_size() override;
    virtual std::tuple<int, int> get_window_pos() override;

    virtual void new_frame() override;
    virtual void present() override;

    virtual bool initialize() override;
    virtual void finalize() override;

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
