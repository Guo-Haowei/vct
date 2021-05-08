#pragma once
#include "Window.h"
#include "renderer/RenderSystem.h"

namespace vct {

class Application {
   public:
    int run();

   private:
    void updateCamera();
    void initialize();
    void userInterface();
    void finalize();

   private:
    Window m_window;
    RenderSystem m_renderSystem;
};

}  // namespace vct
