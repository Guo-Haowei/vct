#pragma once
#include "GlslProgram.h"
#include "application/Window.h"

namespace vct {

struct PerDrawData
{
    GLuint vao;
    // TODO: refactor
    GLuint ebo;
    GLuint vbo1;
    GLuint vbo2;
    unsigned int count;
};

class MainRenderer
{
public:
    void createGpuResources();
    void render();
    void renderBoundingBox(const Matrix4& PV);
    void destroyGpuResources();
    inline void setWindow(Window* pWindow) { m_pWindow = pWindow; }
private:
    Window* m_pWindow;

    // TODO: rename
    GlslProgram m_basicProgram;
    GlslProgram m_boxWireframeProgram;

    PerDrawData m_boxWireframe;
};

} // namespace vct
