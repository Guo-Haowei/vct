#include "ShadowPass.h"
#include "GL/Texture.h"
#include "SceneManager.h"
#define SHADOW_MAP_RESOLUSION 1024

void ShadowPass::initialize()
{    // shader
    ShaderProgram::CreateInfo shaderCreateInfo {};
    shaderCreateInfo.vs = "shadow.vert";
    shaderCreateInfo.fs = "shadow.frag";
    m_shadowShader.reset(new ShaderProgram("shadow", shaderCreateInfo));
    // shadow map
    Texture::CreateInfo shadowMapCreateInfo;
    shadowMapCreateInfo.width = SHADOW_MAP_RESOLUSION;
    shadowMapCreateInfo.height = SHADOW_MAP_RESOLUSION;
    shadowMapCreateInfo.wrapS = shadowMapCreateInfo.wrapT = GL_CLAMP_TO_EDGE;
    shadowMapCreateInfo.minFilter = shadowMapCreateInfo.magFilter = GL_NEAREST;
    g_pShadowMap.reset(new Texture2D("ShadowMap", shadowMapCreateInfo));
    g_pShadowMap->bind();
    g_pShadowMap->texImage2D(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    // fbo
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_pShadowMap->getHandle(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // g_pShadowMap->unbind();

    glUseProgram(m_shadowShader->getHandle());
    auto& light = g_pSceneManager->getScene().light;
    mat4 PV = light.P * light.V;
    m_shadowShader->setUniform("PV", PV);
}

void ShadowPass::render()
{
    glViewport(0, 0, SHADOW_MAP_RESOLUSION, SHADOW_MAP_RESOLUSION);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    // g_pShadowMap->bind();

    glUseProgram(m_shadowShader->getHandle());
    // set camera

    for (auto& mesh : g_pSceneManager->getScene().meshes)
    {
        auto& vao = mesh->vertexArray;
        glBindVertexArray(vao->getHandle());
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowPass::finalize()
{
    glDeleteFramebuffers(1, &m_frameBuffer);
    m_shadowShader->release();
    g_pShadowMap->release();
}
