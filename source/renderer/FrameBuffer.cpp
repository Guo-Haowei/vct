#include "FrameBuffer.h"
#include "GpuTexture.h"

namespace vct {

void DepthBuffer::create(unsigned int width, unsigned int height)
{
    GpuTexture& depthTexture = m_depthTexture;
    depthTexture.m_type = GL_TEXTURE_2D;
    depthTexture.m_format = GL_DEPTH_COMPONENT;
    glGenTextures(1, &depthTexture.m_handle);

    depthTexture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    depthTexture.unbind();

    glGenFramebuffers(1, &m_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.m_handle, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    unbind();
}

void DepthBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

void DepthBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthBuffer::destroy()
{
}

} // namespace vct

