#include "MainPass.h"
#include "SceneManager.h"
#include "math/GeoMath.h"
#include "App.h"

namespace internal {

static void genHexahedronBufferData(std::vector<vec3>& outPositions, std::vector<unsigned int>& outIndices);

} // namespace::internal

void MainPass::initialize()
{
    // scene shader
    {
        ShaderProgram::CreateInfo shaderCreateInfo {};
        shaderCreateInfo.vs = "voxelization.vs.glsl";
        shaderCreateInfo.fs = "voxelization.fs.glsl";
        m_mainShader.reset(new ShaderProgram("voxelization", shaderCreateInfo));
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
        internal::genHexahedronBufferData(positions, indices);
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
    // TODO: rendering to a 3d texture, no need to clear
    int width, height;
    g_pApp->getFrameBufferSize(width, height);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);

    glUseProgram(m_mainShader->getHandle());

    auto& cam = g_pSceneManager->getScene().camera;
    mat4 PV = cam.getP() * cam.getV();
    m_mainShader->setUniform("PV", PV);

    for (auto& mesh : g_pSceneManager->getScene().meshes)
    {
        auto& vao = mesh->vertexArray;
        glBindVertexArray(vao->getHandle());
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }

    // render debug boxes
    // glDisable(GL_DEPTH_TEST); // always on top
    glUseProgram(m_boxShader->getHandle());
    m_boxShader->setUniform("PV", PV);
    glBindVertexArray(m_boxVao->getHandle());
    // glDrawElements(GL_LINES, 32, GL_UNSIGNED_INT, 0);
    glDrawElementsInstanced(GL_LINES, 32, GL_UNSIGNED_INT, 0, m_boxCount);
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

namespace internal {

void genHexahedronBufferData(std::vector<vec3>& outPositions, std::vector<unsigned int>& outIndices)
{
    /**
     *        E__________________ H
     *       /|                 /|
     *      / |                / |
     *     /  |               /  |
     *   A/___|______________/D  |
     *    |   |              |   |
     *    |   |              |   |
     *    |   |              |   |
     *    |  F|______________|___|G
     *    |  /               |  /
     *    | /                | /
     *   B|/_________________|C
     * 
     */
    enum HexahedronVertexIndex { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7 };

    float scale = 0.5f;
    outPositions.clear();
    outPositions = {
        { -scale, +scale, +scale }, // A
        { -scale, -scale, +scale }, // B
        { +scale, -scale, +scale }, // C
        { +scale, +scale, +scale }, // D
        { -scale, +scale, -scale }, // E
        { -scale, -scale, -scale }, // F
        { +scale, -scale, -scale }, // G
        { +scale, +scale, -scale }  // H
    };

    outIndices.clear();
    outIndices = {
        A, B, B, C, C, D, D, A,
        E, F, F, G, G, H, H, E,
        A, E, B, F, D, H, C, G
    };
}

} // namespace::internal
