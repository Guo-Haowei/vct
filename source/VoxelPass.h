#pragma once
#include "GL/ShaderProgram.h"
#include "GL/VertexArray.h"
#include <memory>

class VoxelPass
{
public:
    void initialize();
    void render();
    void finalize();
private:
    std::unique_ptr<ShaderProgram> m_voxelShader;
    std::unique_ptr<GpuBuffer> m_positionBuffer;
    std::unique_ptr<GpuBuffer> m_normalBuffer;
    std::unique_ptr<GpuBuffer> m_indexBuffer;
    std::unique_ptr<VertexArray> m_vertexArray;
};
