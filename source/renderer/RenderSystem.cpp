#include "RenderSystem.h"
#include "MasterRenderer.h"
namespace vct {


void RenderSystem::initialize(Window* pWindow)
{
    m_masterRenderer = new MasterRenderer();
    m_masterRenderer->initialize(pWindow);
}

void RenderSystem::update()
{
    m_masterRenderer->update();
}

void RenderSystem::finalize()
{
    m_masterRenderer->finalize();
    delete m_masterRenderer;
}

} // namespace vct
