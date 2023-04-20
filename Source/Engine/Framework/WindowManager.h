#pragma once
#include <tuple>

#include "ManagerBase.h"

struct GLFWwindow;

class WindowManager : public ManagerBase
{
public:
    WindowManager() : ManagerBase("WindowManager") {}

    bool ShouldClose();
    GLFWwindow* GetRaw();

    std::tuple<int, int> GetFrameSize();
    std::tuple<float, float> GetMousePos();
    bool IsMouseInScreen();

    bool IsKeyDown(int code);

    void NewFrame();
    void Present();

protected:
    bool InitializeInternal();
    void FinalizeInternal();

private:
    static void CursorPosCallback(GLFWwindow* window, double x, double y);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void KeyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    GLFWwindow* mWindow = nullptr;
    struct
    {
        int x, y;
    } mFrameSize;
    struct
    {
        float x, y;
    } mMousePos;
};

extern WindowManager* gWindowManager;
