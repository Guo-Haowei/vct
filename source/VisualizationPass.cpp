#include "VisualizationPass.h"
#include "App.h"
#include "SceneManager.h"
#include "internal/Geometries.h"
#include "common.h"

void VisualizationPass::initialize()
{
    // shader
    ShaderProgram::CreateInfo shaderCreateInfo {};
    shaderCreateInfo.vs = "visualization.vs.glsl";
    shaderCreateInfo.fs = "visualization.fs.glsl";
    m_visualizationShader.reset(new ShaderProgram("visualization", shaderCreateInfo));
    // box buffer
    // create box data (after scene is initialized)
    {
        m_boxVao.reset(new VertexArray("voxel", { GL_TRIANGLES }));
        m_boxVao->bind();
        // mesh data
        std::vector<vec3> positions;
        std::vector<unsigned int> indices;
        internal::genBoxNoNormals(positions, indices);
        {
            GpuBuffer::CreateInfo vertexBufferCreateInfo{};
            vertexBufferCreateInfo.type = GL_ARRAY_BUFFER;
            vertexBufferCreateInfo.usage = GL_STATIC_DRAW;
            vertexBufferCreateInfo.initialBuffer.data = positions.data();
            vertexBufferCreateInfo.initialBuffer.size = sizeof(vec3) * positions.size();
            m_boxVbo.reset(new GpuBuffer("voxel.position", vertexBufferCreateInfo));
            m_boxVao->appendAttribute({ GL_FLOAT, 3, sizeof(vec3) }, *m_boxVbo.get());
        }
        // index buffer
        {
            GpuBuffer::CreateInfo indexBufferCreateInfo{};
            indexBufferCreateInfo.type = GL_ELEMENT_ARRAY_BUFFER;
            indexBufferCreateInfo.usage = GL_STATIC_DRAW;
            indexBufferCreateInfo.initialBuffer.data = indices.data();
            indexBufferCreateInfo.initialBuffer.size = sizeof(unsigned int) * indices.size();
            m_boxEbo.reset(new GpuBuffer("voxel.index", indexBufferCreateInfo));
            m_boxVao->appendIndexBuffer({ GL_UNSIGNED_INT }, *m_boxEbo.get());
        }
        m_boxVao->unbind();
    }
}

void VisualizationPass::render()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    int width, height;
    g_pApp->getFrameBufferSize(width, height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // upload uniforms
    glUseProgram(m_visualizationShader->getHandle());
    auto& cam = g_pSceneManager->getScene().camera;
    mat4 PV = cam.getP() * cam.getV();
    const Box3D& aabb = g_pSceneManager->getScene().aabb;
    const vec3 center = aabb.getCenter();
    const vec3 size = 0.5f * float(1. / VOXEL_SIZE) * aabb.getSize();
    float unitSize = g_pSceneManager->getScene().aabbSizeMax / float(VOXEL_SIZE);
    // float voxelSize = 
    m_visualizationShader->setUniform("PV", PV);
    m_visualizationShader->setUniform("u_voxel_texture", int(0));
    m_visualizationShader->setUniform("u_world_center", center);
    m_visualizationShader->setUniform("u_unit_size", unitSize);
    m_visualizationShader->setUniform("u_voxel_dim", int(VOXEL_SIZE));
    // TODO: extract texture

    glBindVertexArray(m_boxVao->getHandle());
    // glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 1);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, VOXEL_SIZE * VOXEL_SIZE * VOXEL_SIZE);
}

void VisualizationPass::finalize()
{
    m_visualizationShader->release();
    m_boxVao->release();
    m_boxVbo->release();
    m_boxEbo->release();
}
