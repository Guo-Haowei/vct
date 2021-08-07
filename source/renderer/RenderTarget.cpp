#include "RenderTarget.h"

#include "universal/core_assert.h"
#include "universal/print.h"

void RenderTarget::bind()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_handle );
}

void RenderTarget::unbind()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void RenderTarget::destroy()
{
    m_depthAttachment.destroy();
    for ( int i = 0; i < m_colorAttachmentCount; ++i )
        m_colorAttachments[i].destroy();

    glDeleteFramebuffers( 1, &m_handle );
}

void RenderTarget::createDepthAttachment()
{
    Texture2DCreateInfo info{};
    info.width    = m_width;
    info.height   = m_height;
    info.dataType = GL_FLOAT;
    info.format = info.internalFormat = GL_DEPTH_COMPONENT;
    info.minFilter = info.magFilter = GL_NEAREST;
    info.wrapS = info.wrapT = GL_CLAMP_TO_BORDER;

    m_depthAttachment.create2DEmpty( info );

    m_depthAttachment.bind();
    // float border[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border );
    m_depthAttachment.unbind();

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment.GetHandle(), 0 );
}

void RenderTarget::create( int width, int height )
{
    m_width  = width;
    m_height = height;
    glGenFramebuffers( 1, &m_handle );
}

void DepthRenderTarget::create( int width, int height )
{
    RenderTarget::create( width, height );

    bind();
    RenderTarget::createDepthAttachment();
    glDrawBuffer( GL_NONE );
    glReadBuffer( GL_NONE );

    checkError();
    unbind();
}

void RenderTarget::checkError()
{
    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
    {
        Com_PrintError( "Frame buffer not completed" );
    }
}

namespace {
enum { POSISION = 0,
       NORMAL   = 1,
       ALBEDO   = 2 };
}

void GBuffer::create( int width, int height )
{
    RenderTarget::create( width, height );

    m_colorAttachmentCount = 3;

    bind();

    Texture2DCreateInfo info{};
    info.width          = width;
    info.height         = height;
    info.dataType       = GL_FLOAT;
    info.format         = GL_RGBA;
    info.internalFormat = GL_RGBA16F;
    info.minFilter = info.magFilter = GL_NEAREST;

    // position
    m_colorAttachments[POSISION].create2DEmpty( info );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + POSISION,
        GL_TEXTURE_2D,
        m_colorAttachments[POSISION].GetHandle(),
        0 );

    // normal
    m_colorAttachments[NORMAL].create2DEmpty( info );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + NORMAL,
        GL_TEXTURE_2D,
        m_colorAttachments[NORMAL].GetHandle(),
        0 );

    // albedo
    info.internalFormat = GL_RGBA;
    info.dataType       = GL_UNSIGNED_BYTE;
    m_colorAttachments[ALBEDO].create2DEmpty( info );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + ALBEDO,
        GL_TEXTURE_2D,
        m_colorAttachments[ALBEDO].GetHandle(),
        0 );

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers( 3, attachments );

    // depth
    createDepthAttachment();

    checkError();
    unbind();
}
