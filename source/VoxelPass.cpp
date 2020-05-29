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
}

void VoxelPass::render()
{
    // TODO: rendering to a 3d texture, no need to clear
    int width, height;
    g_pApp->getFrameBufferSize(width, height);

    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);

    glUseProgram(m_voxelShader->getHandle());

    auto& cam = g_pSceneManager->getScene().camera;
    mat4 PV = cam.getP() * cam.getV();
    m_voxelShader->setUniform("PV", PV);

    for (auto& mesh : g_pSceneManager->getScene().meshes)
    {
        auto& vao = mesh->vertexArray;
        glBindVertexArray(vao->getHandle());
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }

}

void VoxelPass::finalize()
{
    m_voxelShader->release();
}