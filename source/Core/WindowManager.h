#pragma once
#include "universal/core_math.h"

#include "Base/IManager.h"

struct GLFWwindow;

class WindowManager : public IManager {
public:
    WindowManager()
        : IManager( "WindowManager" )
    {
    }

    virtual bool Init() override;
    virtual void Deinit() override;

    bool ShouldClose();
    ivec2 FrameSize();
    vec2 MousePos();
    bool IsMouseInScreen();

    bool IsKeyDown( int code );
    void NewFrame();
    void Present();

    GLFWwindow* GetHandle();

private:
    GLFWwindow* m_window;
    ivec2 m_frameSize;
    vec2 m_mousePos;
};

extern WindowManager* g_wndMgr;
