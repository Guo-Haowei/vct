#include "VoxelPass.h"
#include "SceneManager.h"
#include "math/GeoMath.h"
#include "App.h"
#include "GL/Texture.h"

void VoxelPass::initialize()
{
    // shader
    ShaderProgram::CreateInfo shaderCreateInfo {};
    shaderCreateInfo.vs = "voxelization.vs.glsl";
    shaderCreateInfo.gs = "voxelization.gs.glsl";
    shaderCreateInfo.fs = "voxelization.fs.glsl";
    m_voxelShader.reset(new ShaderProgram("voxelization", shaderCreateInfo));

    // 3D texture
    // glGenTextures(1, &m_texture3d);
    // glBindTexture(GL_TEXTURE_3D, m_texture3d);

    // // Parameter options.
    // const auto wrap = GL_CLAMP_TO_BORDER;
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);

    // const auto filter = GL_LINEAR_MIPMAP_LINEAR;
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // // Upload texture buffer.
    // const int levels = 5;
    // glTexStorage3D(GL_TEXTURE_3D, levels, GL_RGBA8, VOXEL_SIZE, VOXEL_SIZE, VOXEL_SIZE);
    // //std::vector<GLfloat> data(4 * VOXEL_SIZE * VOXEL_SIZE * VOXEL_SIZE, 0.0f);
    // //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, VOXEL_SIZE, VOXEL_SIZE, VOXEL_SIZE, 0, GL_RGBA, GL_FLOAT, data.data());

    // glGenerateMipmap(GL_TEXTURE_3D);
    // glBindTexture(GL_TEXTURE_3D, 0);
}

void VoxelPass::render()
{
    if (!m_needsUpdate)
        return;
    m_needsUpdate = false;

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

    // upload uniforms
    const Box3D& aabb = g_pSceneManager->getScene().aabb;
    const vec3 center = aabb.getCenter();
    float size = 0.5f * g_pSceneManager->getScene().aabbSizeMax;

    m_voxelShader->setUniform("u_voxel_texture", int(VOXEL_TEXTURE_DEFAULT_SLOT));
    m_voxelShader->setUniform("u_world_center", center);
    m_voxelShader->setUniform("u_world_size_half", size);

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
}

void VoxelPass::finalize()
{
    m_voxelShader->release();
}

void VoxelPass::clearTexture()
{
    if (!m_needsUpdate)
        return;
    glBindTexture(GL_TEXTURE_3D, m_texture3d);
    float clearColor[4] = { 0.f, 0.f, 0.f, 0.f };
    glClearTexImage(m_texture3d, 0, GL_RGBA, GL_FLOAT, &clearColor);
}

std::unique_ptr<Texture3D> g_pVoxelTexture;
