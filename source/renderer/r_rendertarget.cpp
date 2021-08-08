#include "r_rendertarget.h"

#include "common/com_dvars.h"
#include "common/main_window.h"
#include "r_cbuffers.h"
#include "universal/core_assert.h"
#include "universal/dvar_api.h"
#include "universal/print.h"
#include "universal/universal.h"

void RenderTarget::Bind()
{
    glBindFramebuffer( GL_FRAMEBUFFER, mHandle );
}

void RenderTarget::Unbind()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void RenderTarget::Destroy()
{
    mDepthAttachment.destroy();
    for ( int i = 0; i < mColorAttachmentCount; ++i )
    {
        mColorAttachments[i].destroy();
    }

    glDeleteFramebuffers( 1, &mHandle );
    mHandle = 0;
}

void RenderTarget::CreateDepthAttachment()
{
    Texture2DCreateInfo info{};
    info.width    = mWidth;
    info.height   = mHeight;
    info.dataType = GL_FLOAT;
    info.format = info.internalFormat = GL_DEPTH_COMPONENT;
    info.minFilter = info.magFilter = GL_NEAREST;
    info.wrapS = info.wrapT = GL_CLAMP_TO_BORDER;

    mDepthAttachment.create2DEmpty( info );

    mDepthAttachment.bind();
    // float border[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border );
    mDepthAttachment.unbind();

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthAttachment.GetHandle(), 0 );
}

void RenderTarget::Create( int width, int height )
{
    mWidth  = width;
    mHeight = height;
    glGenFramebuffers( 1, &mHandle );
}

void DepthRenderTarget::Create( int width, int height )
{
    RenderTarget::Create( width, height );

    Bind();
    RenderTarget::CreateDepthAttachment();
    glDrawBuffer( GL_NONE );
    glReadBuffer( GL_NONE );

    CheckError();
    Unbind();
}

void RenderTarget::CheckError()
{
    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
    {
        Com_PrintError( "Frame buffer not completed" );
    }
}

namespace {
enum { POSISION = 0,
       NORMAL   = 1,
       ALBEDO   = 2,
       SSAO     = 0 };
}

void GBuffer::Create( int width, int height )
{
    RenderTarget::Create( width, height );

    mColorAttachmentCount = 3;

    Bind();

    Texture2DCreateInfo info{};
    info.width          = width;
    info.height         = height;
    info.dataType       = GL_FLOAT;
    info.format         = GL_RGBA;
    info.internalFormat = GL_RGBA16F;
    info.minFilter = info.magFilter = GL_NEAREST;

    // position
    mColorAttachments[POSISION].create2DEmpty( info );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + POSISION,
        GL_TEXTURE_2D,
        mColorAttachments[POSISION].GetHandle(),
        0 );

    // normal
    mColorAttachments[NORMAL].create2DEmpty( info );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + NORMAL,
        GL_TEXTURE_2D,
        mColorAttachments[NORMAL].GetHandle(),
        0 );

    // albedo
    info.internalFormat = GL_RGBA;
    info.dataType       = GL_UNSIGNED_BYTE;
    mColorAttachments[ALBEDO].create2DEmpty( info );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + ALBEDO,
        GL_TEXTURE_2D,
        mColorAttachments[ALBEDO].GetHandle(),
        0 );

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers( 3, attachments );

    // depth
    CreateDepthAttachment();

    CheckError();
    Unbind();
}

void SSAOBuffer::Create( int width, int height )
{
    RenderTarget::Create( width, height );

    mColorAttachmentCount = 1;

    Bind();

    Texture2DCreateInfo info{};
    info.width     = width;
    info.height    = height;
    info.minFilter = info.magFilter = GL_NEAREST;

    // info.dataType       = GL_UNSIGNED_BYTE;
    // info.format         = GL_RGBA;
    // info.internalFormat = GL_RGBA;
    info.dataType       = GL_FLOAT;
    info.format         = GL_RED;
    info.internalFormat = GL_R32F;

    // position
    mColorAttachments[SSAO].create2DEmpty( info );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + SSAO,
        GL_TEXTURE_2D,
        mColorAttachments[SSAO].GetHandle(),
        0 );

    GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers( 1, attachments );

    CheckError();
    Unbind();
}

DepthRenderTarget g_shadowRT;
GBuffer g_gbufferRT;
SSAOBuffer g_ssaoRT;

void R_CreateRT()
{
    const ivec2 extent = MainWindow::FrameSize();

    const int res = Dvar_GetInt( r_shadowRes );
    core_assert( is_power_of_two( res ) );

    g_shadowRT.Create( NUM_CASCADES * res, res );
    g_gbufferRT.Create( extent.x, extent.y );
    g_ssaoRT.Create( extent.x, extent.y );
}

void R_DestroyRT()
{
    g_shadowRT.Destroy();
    g_gbufferRT.Destroy();
    g_ssaoRT.Destroy();
}