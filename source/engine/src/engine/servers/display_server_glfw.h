#pragma once
#include "core/input/input_code.h"
#include "display_server.h"

struct GLFWwindow;

namespace vct {

class DisplayServerGLFW : public DisplayServer {
public:
    bool should_close() override;

    std::tuple<int, int> get_frame_size() override;
    std::tuple<int, int> get_window_pos() override;

    void new_frame() override;
    void present() override;

    bool initialize() override;
    void finalize() override;

private:
    void initialize_key_mapping();

    static void cursor_pos_cb(GLFWwindow* window, double x, double y);
    static void mouse_button_cb(GLFWwindow* window, int button, int action, int mods);
    static void key_cb(GLFWwindow* window, int keycode, int scancode, int action, int mods);
    static void scroll_cb(GLFWwindow* window, double xoffset, double yoffset);

    GLFWwindow* m_window = nullptr;
    struct {
        int x, y;
    } m_frame_size, m_window_pos;

    inline static std::unordered_map<int, KeyCode> s_key_mapping;
};

}  // namespace vct
