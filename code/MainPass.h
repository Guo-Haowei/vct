#pragma once
#include "GL/ShaderProgram.h"
#include "GL/VertexArray.h"
#include <memory>

class MainPass
{
public:
    void initialize();
    void render();
    void finalize();
private:
    std::unique_ptr<ShaderProgram> m_mainShader;
    std::unique_ptr<ShaderProgram> m_boxShader;
    std::unique_ptr<VertexArray> m_boxVao;
    std::unique_ptr<GpuBuffer> m_boxVbo;
    std::unique_ptr<GpuBuffer> m_boxEbo;
    std::unique_ptr<GpuBuffer> m_boxCenter;
    std::unique_ptr<GpuBuffer> m_boxSize;

    unsigned int m_boxCount = 1;
};
