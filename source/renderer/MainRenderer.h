#pragma once
#include "GlslProgram.h"
#include "GpuTexture.h"
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
    void renderVoxels(const Matrix4& PV);
    void renderSceneNoGI(const Matrix4& PV);
    void renderVoxelTexture();
    void destroyGpuResources();
    inline void setWindow(Window* pWindow) { m_pWindow = pWindow; }
private:
    Window* m_pWindow;

    /// shader programs
    GlslProgram m_voxelProgram;
    GlslProgram m_visualizeProgram;
    GlslProgram m_basicProgram;
    GlslProgram m_boxWireframeProgram;

    /// vertex arrays
    PerDrawData m_boxWireframe;
    PerDrawData m_box; // no normals

    /// textures
    GpuTexture m_albedoVoxel;
};

} // namespace vct
