#include "TextureRenderer.h"
#include "App.h"

void TextureRenderer::initialize()
{
    // shader
    ShaderProgram::CreateInfo shaderCreateInfo {};
    shaderCreateInfo.vs = "quad.vs.glsl";
    shaderCreateInfo.fs = "quad.fs.glsl";
    m_quadShader.reset(new ShaderProgram("quad", shaderCreateInfo));
    // quad
    m_quadVao.reset(new VertexArray("quad", { GL_TRIANGLES }));
    m_quadVao->bind();
    // mesh data
    float s = 1.f;
    float positions[8] = {
        -s,  s, // top left
        -s, -s, // bottom left
         s, -s, // bottom right
         s,  s  // top right
    };
    unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    // vertex buffer
    GpuBuffer::CreateInfo vertexBufferCreateInfo {};
    vertexBufferCreateInfo.type = GL_ARRAY_BUFFER;
    vertexBufferCreateInfo.usage = GL_STATIC_DRAW;
    vertexBufferCreateInfo.initialBuffer.data = positions;
    vertexBufferCreateInfo.initialBuffer.size = sizeof(positions);
    m_quadVbo.reset(new GpuBuffer("box.position", vertexBufferCreateInfo));
    m_quadVao->appendAttribute({GL_FLOAT, 2, 2 * sizeof(float)}, *m_quadVbo.get());
    // index buffer
    GpuBuffer::CreateInfo indexBufferCreateInfo{};
    indexBufferCreateInfo.type = GL_ELEMENT_ARRAY_BUFFER;
    indexBufferCreateInfo.usage = GL_STATIC_DRAW;
    indexBufferCreateInfo.initialBuffer.data = indices;
    indexBufferCreateInfo.initialBuffer.size = sizeof(indices);
    m_quadEbo.reset(new GpuBuffer("box.index", indexBufferCreateInfo));
    m_quadVao->appendIndexBuffer({GL_UNSIGNED_INT}, *m_quadEbo.get());
    m_quadVao->unbind();
}

void TextureRenderer::render()
{
    // glViewport()
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(m_quadShader->getHandle());
    glBindVertexArray(m_quadVao->getHandle());
    renderTexture(0, 1);
}

void TextureRenderer::renderTexture(int texture, int slot)
{
    int width, height;
    g_pApp->getFrameBufferSize(width, height);
    glViewport(0.9 * width, (10 - slot) / 10. * height, 0.1 * width, 0.1 * height);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void TextureRenderer::finalize()
{
    m_quadShader->release();
    m_quadVao->release();
    m_quadVbo->release();
    m_quadEbo->release();
}
