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
