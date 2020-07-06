#include "VoxelPass.h"
#include "SceneManager.h"
#include "math/GeoMath.h"
#include "App.h"
#include "GL/Texture.h"

void VoxelPass::initialize()
{
    // shader
    ShaderProgram::CreateInfo shaderCreateInfo {};
    shaderCreateInfo.vs = "voxelization.vert";
    shaderCreateInfo.gs = "voxelization.geom";
    shaderCreateInfo.fs = "voxelization.frag";
    m_voxelShader.reset(new ShaderProgram("voxelization", shaderCreateInfo));

    // upload uniforms
    glUseProgram(m_voxelShader->getHandle());
    const Box3D& aabb = g_pSceneManager->getScene().aabb;
    const vec3 center = aabb.getCenter();
    float size = 0.5f * g_pSceneManager->getScene().aabbSizeMax;

    m_voxelShader->setUniform("u_voxel_texture", int(VOXEL_TEXTURE_DEFAULT_SLOT));
    m_voxelShader->setUniform("u_world_center", center);
    m_voxelShader->setUniform("u_world_size_half", size);
    m_voxelShader->setUniform("u_voxel_dim", int(VOXEL_SIZE));
    m_voxelShader->setUniform("u_light_pos", g_pSceneManager->getScene().light.position);
}

void VoxelPass::render()
{
    if (!m_needsUpdate)
        return;
    m_needsUpdate = false;
    // glSubpixelPrecisionBiasNV(8, 8);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glViewport(0, 0, VOXEL_SIZE, VOXEL_SIZE);
    glUseProgram(m_voxelShader->getHandle());
    // activate texture
    g_pVoxelTexture->bindToSlotForWrite(VOXEL_TEXTURE_DEFAULT_SLOT);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_3D, m_texture3d);
    // glBindImageTexture(0, m_texture3d, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    for (auto& mesh : g_pSceneManager->getScene().meshes)
    {
        // bind texture
        auto& albedo = g_pSceneManager->getScene().materials.at(mesh->materialIndex)->albedo;
        if (albedo != nullptr)
        {
            albedo->bindToSlot(0);
        }
        m_voxelShader->setUniform("u_albedo", 0);
        auto& vao = mesh->vertexArray;
        glBindVertexArray(vao->getHandle());
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }

    g_pVoxelTexture->generateMipMap();
    g_pVoxelTexture->unbind();

    // reset color mask
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // clear texture after use
    // glSubpixelPrecisionBiasNV(0, 0);
}

void VoxelPass::finalize()
{
    m_voxelShader->release();
}

void VoxelPass::clearTexture()
{
    if (!m_needsUpdate)
        return;
    float clearColor[4] = { 0.f, 0.f, 0.f, 0.f };
    g_pVoxelTexture->clear(clearColor);
}
