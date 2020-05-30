#pragma once
#include "common.h"
#include "GL/ShaderProgram.h"
#include "GL/VertexArray.h"
#include <memory>

class VoxelPass
{
public:
    void initialize();
    void render();
    void finalize();
    void clearTexture();
private:

    std::unique_ptr<ShaderProgram> m_voxelShader;

    GLuint m_texture3d;

    bool m_needsUpdate = true;
};
