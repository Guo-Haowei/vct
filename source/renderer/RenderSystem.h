#pragma once

namespace vct {

class Window;
class MasterRenderer;

class RenderSystem
{
public:
    void initialize(Window* pWindow);
    void finalize();
    void update();
private:
    MasterRenderer* m_masterRenderer;
};

} // namespace vct
