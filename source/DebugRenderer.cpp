#include "DebugRenderer.h"
#include "App.h"
#include "SceneManager.h"
#include "GL/Texture.h"
#include "common.h"

void DebugRenderer::initialize()
{
    // quad
    {
        // shader
        ShaderProgram::CreateInfo shaderCreateInfo{};
        shaderCreateInfo.vs = "quad.vs.glsl";
        shaderCreateInfo.fs = "quad.fs.glsl";
        m_quadShader.reset(new ShaderProgram("quad", shaderCreateInfo));
        // quad
        m_quadVao.reset(new VertexArray("quad", {GL_TRIANGLES}));
        m_quadVao->bind();
        // mesh data
        float s = 1.f;
        float positions[8] = {
            -s, s,  // top left
            -s, -s, // bottom left
            s, -s,  // bottom right
            s, s    // top right
        };
        unsigned int indices[6] = {
            0, 1, 2,
            0, 2, 3};
        // vertex buffer
        GpuBuffer::CreateInfo vertexBufferCreateInfo{};
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
    // axis
    {
        // shader
        ShaderProgram::CreateInfo shaderCreateInfo{};
        shaderCreateInfo.vs = "line3d.vs.glsl";
        shaderCreateInfo.fs = "line3d.fs.glsl";
        m_axisShader.reset(new ShaderProgram("axis", shaderCreateInfo));
        // lines
        m_axisVao.reset(new VertexArray("axis", { GL_LINES }));
        m_axisVao->bind();
        // mesh data
        struct Pos { float x, y, z; };
        struct Color { float r, g, b; };
        struct Vertex { Pos pos; Color color; };
        Vertex positions[6] = {
            { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
            { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
        };
        // vertex buffer
        GpuBuffer::CreateInfo vertexBufferCreateInfo{};
        vertexBufferCreateInfo.type = GL_ARRAY_BUFFER;
        vertexBufferCreateInfo.usage = GL_STATIC_DRAW;
        vertexBufferCreateInfo.initialBuffer.data = positions;
        vertexBufferCreateInfo.initialBuffer.size = sizeof(positions);
        m_axisVbo.reset(new GpuBuffer("axis.vertex", vertexBufferCreateInfo));
        m_axisVao->appendAttribute({GL_FLOAT, 3, sizeof(Vertex)}, *m_axisVbo.get());
        m_axisVao->appendAttribute({GL_FLOAT, 3, sizeof(Vertex), sizeof(Pos)}, *m_axisVbo.get());
        m_axisVao->unbind();
    }

    g_pApp->getFrameBufferSize(m_width, m_height);
}

void DebugRenderer::render()
{
    int width = m_width, height = m_height;
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(m_quadShader->getHandle());
    glBindVertexArray(m_quadVao->getHandle());
    renderTexture(g_pShadowMap.get(), SHADOW_MAP_DEFAULT_SLOT, 1);

    glUseProgram(m_axisShader->getHandle());
    auto& cam = g_pSceneManager->getScene().camera;
    mat4 PV = cam.getP() * cam.getV();
    m_axisShader->setUniform("PV", PV);
    glBindVertexArray(m_axisVao->getHandle());
    glViewport(width / 2, 0, width / 2, height);
    glDrawArrays(GL_LINES, 0, 6);
    glViewport(0, 0, width / 2, height);
    glDrawArrays(GL_LINES, 0, 6);
}

void DebugRenderer::renderTexture(Texture* texture, int slot, int index)
{
    static double s = .15;
    texture->bindToSlot(slot);
    m_quadShader->setUniform("u_texture", slot);
    glViewport((1 - s) * m_width, (1 / s - index) / (1 / s) * m_height, s * m_width, s * m_height);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void DebugRenderer::finalize()
{
    m_quadShader->release();
    m_quadVao->release();
    m_quadVbo->release();
    m_quadEbo->release();

    m_axisShader->release();
    m_axisVao->release();
    m_axisVbo->release();
}
