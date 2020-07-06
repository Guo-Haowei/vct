#pragma once
#include "GlslProgram.h"
#include "application/Window.h"

namespace vct {

class MainRenderer
{
public:
    void createGpuResources();
    void render();
    void destroyGpuResources();
    inline void setWindow(Window* pWindow) { m_pWindow = pWindow; }
private:
    GlslProgram m_basic;
    Window* m_pWindow;
};

} // namespace vct
