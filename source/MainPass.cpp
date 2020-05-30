#include "MainPass.h"
#include "SceneManager.h"
#include "math/GeoMath.h"
#include "internal/Geometries.h"
#include "App.h"


void MainPass::initialize()
{
    // scene shader
    {
        ShaderProgram::CreateInfo shaderCreateInfo {};
        shaderCreateInfo.vs = "main.vs.glsl";
        shaderCreateInfo.fs = "main.fs.glsl";
        m_mainShader.reset(new ShaderProgram("main", shaderCreateInfo));
    }
    // box shader
    {
        ShaderProgram::CreateInfo shaderCreateInfo {};
        shaderCreateInfo.vs = "bbox.vs.glsl";
        shaderCreateInfo.fs = "bbox.fs.glsl";
        m_boxShader.reset(new ShaderProgram("bbox", shaderCreateInfo));
    }
    // create box data (after scene is initialized)
    {
        m_boxVao.reset(new VertexArray("boxes", { GL_LINES }));
        m_boxVao->bind();
        // mesh data
        std::vector<vec3> positions;
        std::vector<unsigned int> indices;
        internal::genBoxWireframe(positions, indices);
        {
            GpuBuffer::CreateInfo vertexBufferCreateInfo {};
            vertexBufferCreateInfo.type = GL_ARRAY_BUFFER;
            vertexBufferCreateInfo.usage = GL_STATIC_DRAW;
            vertexBufferCreateInfo.initialBuffer.data = positions.data(); 
            vertexBufferCreateInfo.initialBuffer.size = sizeof(vec3) * positions.size();
            m_boxVbo.reset(new GpuBuffer("box.position", vertexBufferCreateInfo));
            m_boxVao->appendAttribute({ GL_FLOAT, 3, sizeof(vec3) }, *m_boxVbo.get());
        }
        // instance data
        m_boxCount += g_pSceneManager->getScene().meshes.size();
        std::vector<vec3> centers, sizes;
        centers.reserve(m_boxCount); sizes.reserve(m_boxCount);
        for (auto& mesh : g_pSceneManager->getScene().meshes)
        {
            centers.push_back(mesh->aabb.getCenter());
            sizes.push_back(mesh->aabb.getSize());
        }
        {
            GpuBuffer::CreateInfo centerBufferCreateInfo {};
            centerBufferCreateInfo.type = GL_ARRAY_BUFFER;
            centerBufferCreateInfo.usage = GL_STATIC_DRAW;
            centerBufferCreateInfo.initialBuffer.data = centers.data(); 
            centerBufferCreateInfo.initialBuffer.size = sizeof(vec3) * centers.size();
            m_boxCenter.reset(new GpuBuffer("box.center", centerBufferCreateInfo));
            m_boxVao->appendAttribute({ GL_FLOAT, 3, sizeof(vec3), 0, 1 }, *m_boxCenter.get());
        }
        {
            GpuBuffer::CreateInfo sizeBufferCreateInfo {};
            sizeBufferCreateInfo.type = GL_ARRAY_BUFFER;
            sizeBufferCreateInfo.usage = GL_STATIC_DRAW;
            sizeBufferCreateInfo.initialBuffer.data = sizes.data(); 
            sizeBufferCreateInfo.initialBuffer.size = sizeof(vec3) * sizes.size();
            m_boxSize.reset(new GpuBuffer("box.center", sizeBufferCreateInfo));
            m_boxVao->appendAttribute({ GL_FLOAT, 3, sizeof(vec3), 0, 1 }, *m_boxSize.get());
        }
        // index buffer
        {
            GpuBuffer::CreateInfo indexBufferCreateInfo {};
            indexBufferCreateInfo.type = GL_ELEMENT_ARRAY_BUFFER;
            indexBufferCreateInfo.usage = GL_STATIC_DRAW;
            indexBufferCreateInfo.initialBuffer.data = indices.data(); 
            indexBufferCreateInfo.initialBuffer.size = sizeof(unsigned int) * indices.size();
            m_boxEbo.reset(new GpuBuffer("box.index", indexBufferCreateInfo));
            m_boxVao->appendIndexBuffer({ GL_UNSIGNED_INT }, *m_boxEbo.get());
        }
        m_boxVao->unbind();
    }
}

void MainPass::render()
{
    auto& cam = g_pSceneManager->getScene().camera;
    int width, height;
    g_pApp->getFrameBufferSize(width, height);
    // glViewport(0, 0, width, height);
    glViewport(width / 2, 0, width / 2, height);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // scene
    // upload uniforms
    glUseProgram(m_mainShader->getHandle());
    mat4 PV = cam.getP() * cam.getV();
    m_mainShader->setUniform("PV", PV);

    for (auto& mesh : g_pSceneManager->getScene().meshes)
    {
        // bind texture
        auto& albedo = g_pSceneManager->getScene().materials.at(mesh->materialIndex)->albedo;
        if (albedo != nullptr)
        {
            albedo->bindToSlot(0);
        }
        m_mainShader->setUniform("u_albedo", 0);
        auto& vao = mesh->vertexArray;
        glBindVertexArray(vao->getHandle());
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }

#if 0
    // debug box
    // render debug boxes
    glUseProgram(m_boxShader->getHandle());
    m_boxShader->setUniform("PV", PV);
    glBindVertexArray(m_boxVao->getHandle());
    // glDrawElements(GL_LINES, 32, GL_UNSIGNED_INT, 0);
    glDrawElementsInstanced(GL_LINES, 32, GL_UNSIGNED_INT, 0, m_boxCount);
#endif
}

void MainPass::finalize()
{
    m_mainShader->release();
    m_boxShader->release();
    m_boxVao->release();
    m_boxVbo->release();
    m_boxEbo->release();
    m_boxCenter->release();
    m_boxSize->release();
}
