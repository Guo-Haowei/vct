#pragma once
#include "ImguiRenderer.h"
#include "MainRenderer.h"
#include "application/Window.h"

namespace vct {

class MasterRenderer
{
public:
    void initialize(Window* pWindow);
    void finalize();
    void update();
private:
    Window*         m_pWindow;
    ImguiRenderer   m_imguiRenderer;
    MainRenderer    m_mainRenderer;
};

} // namespace vct

