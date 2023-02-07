#pragma once

#include "Manager/BaseApplication.hpp"

#include "imgui/imgui.h"

struct GLFWwindow;

class GlfwApplication : public BaseApplication {
public:
    using BaseApplication::BaseApplication;

    virtual void Finalize() override;

    virtual void Tick() override;

    virtual bool ShouldQuit() override;

    virtual void* GetMainWindowHandler() override { return m_pGlfwWindow; };

    virtual bool CreateMainWindow() override;

    virtual void GetFramebufferSize( int& width, int& height ) override;

protected:
    bool CreateMainWindowInternal( bool isOpenGL );

    void ResizeWindow( int new_width, int new_height );

protected:
    GLFWwindow* m_pGlfwWindow{ nullptr };
    // bool m_bInLeftDrag = false;
    // bool m_bInRightDrag = false;
    // int m_iPreviousX = 0;
    // int m_iPreviousY = 0;
};