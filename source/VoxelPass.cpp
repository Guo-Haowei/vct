#include "VoxelPass.h"
#include "SceneManager.h"
#include "math/GeoMath.h"
#include "App.h"

void VoxelPass::initialize()
{
    // shader
    ShaderProgram::CreateInfo shaderCreateInfo {};
    shaderCreateInfo.vs = "voxelization.vs.glsl";
    shaderCreateInfo.fs = "voxelization.fs.glsl";
    m_voxelShader.reset(new ShaderProgram("voxelization", shaderCreateInfo));

    auto& mesh = g_pSceneManager->getScene().meshes.front();

    // position buffer
    GpuBuffer::CreateInfo vertexBufferCreateInfo {};
    vertexBufferCreateInfo.type = GL_ARRAY_BUFFER;
    vertexBufferCreateInfo.usage = GL_STATIC_DRAW;
    vertexBufferCreateInfo.initialBuffer.data = mesh->positions.data(); 
    vertexBufferCreateInfo.initialBuffer.size = sizeof(vec3) * mesh->positions.size();
    m_positionBuffer.reset(new GpuBuffer("mesh.positions", vertexBufferCreateInfo));
    // normal buffer
    GpuBuffer::CreateInfo normalBufferCreateInfo {};
    normalBufferCreateInfo.type = GL_ARRAY_BUFFER;
    normalBufferCreateInfo.usage = GL_STATIC_DRAW;
    normalBufferCreateInfo.initialBuffer.data = mesh->normals.data();
    normalBufferCreateInfo.initialBuffer.size = sizeof(vec3) * mesh->normals.size();
    m_normalBuffer.reset(new GpuBuffer("mesh.colors", normalBufferCreateInfo));
    // index array
    GpuBuffer::CreateInfo indexBufferCreateInfo {};
    indexBufferCreateInfo.type = GL_ELEMENT_ARRAY_BUFFER;
    indexBufferCreateInfo.usage = GL_STATIC_DRAW;
    indexBufferCreateInfo.initialBuffer.data = mesh->indices.data(); 
    indexBufferCreateInfo.initialBuffer.size = sizeof(unsigned int) * mesh->indices.size();
    m_indexBuffer.reset(new GpuBuffer("mesh.indices", indexBufferCreateInfo));
    // vertex array
    m_vertexArray.reset(new VertexArray("mesh.vao", { GL_TRIANGLES }));
    m_vertexArray->bind()
        .appendAttribute({ GL_FLOAT, 3, sizeof(vec3), 0 }, *m_positionBuffer.get())
        .appendAttribute({ GL_FLOAT, 3, sizeof(vec3), 0 }, *m_normalBuffer.get())
        .appendIndexBuffer({ GL_UNSIGNED_INT }, *m_indexBuffer.get())
        .unbind();
}

void VoxelPass::render()
{
    // TODO: rendering to a 3d texture, no need to clear
    int width, height;
    g_pApp->getFrameBufferSize(width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);

    glUseProgram(m_voxelShader->getHandle());

    auto& cam = g_pSceneManager->getScene().camera;
    mat4 PV = cam.getP() * cam.getV();
    m_voxelShader->setUniform("PV", PV);

    glBindVertexArray(m_vertexArray->getHandle());
    auto& mesh = g_pSceneManager->getScene().meshes.front();
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
}

void VoxelPass::finalize()
{
    m_voxelShader->release();
    m_positionBuffer->release();
    m_normalBuffer->release();
    m_indexBuffer->release();
    m_vertexArray->release();
}