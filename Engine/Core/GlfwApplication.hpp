#pragma once

#include "Manager/BaseApplication.hpp"

#include "GeomMath.hpp"
#include "BaseManager.hpp"
#include "imgui/imgui.h"

struct GLFWwindow;

class GlfwApplication : public BaseApplication {
public:
    virtual void Finalize() override;

    virtual void Tick() override;

    virtual bool ShouldQuit() override;

    virtual void* GetMainWindowHandler() override { return m_pGlfwWindow; };

    virtual bool CreateMainWindow() override;

    virtual void GetFramebufferSize( uint32_t& width, uint32_t& height ) override;

protected:
    // virtual void onWindowResize( int new_width, int new_height );

protected:
    GLFWwindow* m_pGlfwWindow{ nullptr };
    // bool m_bInLeftDrag = false;
    // bool m_bInRightDrag = false;
    // int m_iPreviousX = 0;
    // int m_iPreviousY = 0;

    int m_width;
    int m_height;
    std::string m_title;
};