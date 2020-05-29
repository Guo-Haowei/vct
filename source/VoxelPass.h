#pragma once
#include "GL/ShaderProgram.h"
#include "GL/VertexArray.h"
#include <memory>

#define VOXEL_SIZE 128

class VoxelPass
{
public:
    void initialize();
    void render();
    void finalize();
private:
    void clearTexture();

    std::unique_ptr<ShaderProgram> m_voxelShader;

    GLuint m_texture3d;
};
