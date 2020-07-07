#pragma once
#include "base/GeoMath.h"
#include <array>

struct GLFWwindow;
struct GLFWcursor;

namespace vct {

class Window
{
    enum { MOUSE_BUTTON_COUNT = 5 }; // ImGuiMouseButton_COUNT
    enum { MOUSE_CURSOR_COUNT = 9 }; // ImGuiMouseCursor_COUNT
public:
    void initialize();
    void finalize();
    bool shouldClose();
    void pollEvents();
    void swapBuffers();
    void beginFrame();

    bool isKeyDown(int key) const;
    Extent2i getWindowExtent() const;
    Extent2i getFrameExtent() const;
    inline double getTime() const { return m_time; }
    inline GLFWwindow* getGlfwWindow() { return m_pGlfwWindow; }
private:
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void charCallback(GLFWwindow* window, unsigned int c);
private:
    GLFWwindow*                                     m_pGlfwWindow;
    double                                          m_time;
    std::array<bool, MOUSE_BUTTON_COUNT>            m_mouseJustPressed;
    std::array<GLFWcursor*, MOUSE_CURSOR_COUNT>     m_mouseCursors;

    Extent2i                                        m_extent;
};

} // namespace vct
