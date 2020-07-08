#pragma once
#include "GlslProgram.h"
#include "GpuTexture.h"
#include "application/Window.h"

namespace vct {

struct MeshData
{
    GLuint vao          = 0;
    GLuint ebo          = 0;
    GLuint vbos[3]      = { 0, 0, 0 };
    unsigned int count  = 0;
};

struct MaterialData
{
    Vector4 albedoColor;
    GpuTexture albedoMap;
    bool hasAlbedoMap;
    // specular...
    // normal...
};

class MainRenderer
{
public:
    void createGpuResources();
    void render();
    void renderBoundingBox(const Matrix4& PV);
    void visualizeVoxels(const Matrix4& PV);
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
    GlslProgram m_voxelPostProgram;

    /// vertex arrays
    MeshData m_boxWireframe;
    MeshData m_box; // no normals

    /// textures
    GpuTexture m_albedoVoxel;
    GpuTexture m_normalVoxel;
};

} // namespace vct
