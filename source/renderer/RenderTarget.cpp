#include "RenderTarget.h"
#include "base/Exception.h"
#include "base/Assertion.h"

namespace vct {

void RenderTarget::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

void RenderTarget::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::destroy()
{
    m_depthAttachment.destroy();
    for (int i = 0; i < m_colorAttachmentCount; ++i)
        m_colorAttachments[i].destroy();

    glDeleteFramebuffers(1, &m_handle);
}

void RenderTarget::createAttachment(GpuTexture& texture, int attachment, const Texture2DCreateInfo& info)
{
    // texture.create2DEmpty();
}

void RenderTarget::create(int width, int height)
{
    m_width = width;
    m_height = height;
    glGenFramebuffers(1, &m_handle);
}

void DepthRenderTarget::create(int width, int height)
{
    RenderTarget::create(width, height);

    Texture2DCreateInfo info {};
    info.width = width;
    info.height = height;
    info.dataType = GL_FLOAT;
    info.format = info.internalFormat = GL_DEPTH_COMPONENT;
    info.minFilter = info.magFilter = GL_NEAREST;
    info.wrapS = info.wrapT = GL_CLAMP_TO_EDGE;

    m_depthAttachment.create2DEmpty(info);

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment.m_handle, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    checkError();
    unbind();
}

void RenderTarget::checkError()
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        THROW_EXCEPTION("Frame buffer not completed");
}

namespace {
    enum { POSISION = 0, NORMAL = 1, ALBEDO = 2 };
}

void GBuffer::create(int width, int height)
{
    RenderTarget::create(width, height);

    m_colorAttachmentCount = 3;

    bind();

    Texture2DCreateInfo info {};
    info.width = width;
    info.height = height;
    info.dataType = GL_FLOAT;
    info.format = GL_RGBA;
    info.internalFormat = GL_RGBA16F;
    info.minFilter = info.magFilter = GL_NEAREST;

    // position
    m_colorAttachments[POSISION].create2DEmpty(info);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + POSISION,
        GL_TEXTURE_2D,
        m_colorAttachments[POSISION].getHandle(),
        0);

    // normal
    m_colorAttachments[NORMAL].create2DEmpty(info);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + NORMAL,
        GL_TEXTURE_2D,
        m_colorAttachments[NORMAL].getHandle(),
        0);

    // albedo
    info.internalFormat = GL_RGBA;
    info.dataType = GL_UNSIGNED_BYTE;
    m_colorAttachments[ALBEDO].create2DEmpty(info);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + ALBEDO,
        GL_TEXTURE_2D,
        m_colorAttachments[ALBEDO].getHandle(),
        0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    ASSERT(m_depthAttachment.getHandle() == 0);

    glGenRenderbuffers(1, &m_rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);

    checkError();
    unbind();
}

} // namespace vct


