#pragma once
#include "GL/ShaderProgram.h"
#include "GL/VertexArray.h"
#include <memory>

class TextureRenderer
{
public:
    void initialize();
    void render();
    void finalize();
private:
    void renderTexture(int texture, int slot);

    std::unique_ptr<ShaderProgram> m_quadShader;
    std::unique_ptr<VertexArray> m_quadVao;
    std::unique_ptr<GpuBuffer> m_quadVbo;
    std::unique_ptr<GpuBuffer> m_quadEbo;
};

