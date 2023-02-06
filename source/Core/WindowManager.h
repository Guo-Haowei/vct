#pragma once

#include "GeomMath.hpp"
#include "BaseManager.hpp"
#include "imgui/imgui.h"

struct GLFWwindow;

class WindowManager : public BaseManager {
public:
    WindowManager()
        : BaseManager( "WindowManager" )
    {
    }

    virtual bool Initialize() override;
    virtual void Finalize() override;

    bool ShouldClose();
    ivec2 FrameSize();
    vec2 MousePos();
    bool IsMouseInScreen();

    bool IsKeyDown( ImGuiKey code );
    void NewFrame();

    GLFWwindow* GetHandle();

private:
    GLFWwindow* m_window;
    ivec2 m_frameSize;
    vec2 m_mousePos;
};

extern WindowManager* g_wndMgr;
