#include "r_rendertarget.h"

#include "Core/com_dvars.h"
#include "Core/main_window.h"
#include "r_cbuffers.h"
#include "Core/Check.h"
#include "universal/dvar_api.h"
#include "Core/Log.h"
#include "universal/universal.h"

void RenderTarget::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
}

void RenderTarget::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::Destroy()
{
    mDepthAttachment.destroy();
    for (int i = 0; i < mColorAttachmentCount; ++i)
    {
        mColorAttachments[i].destroy();
    }

    glDeleteFramebuffers(1, &mHandle);
    mHandle = 0;
}

void RenderTarget::CreateDepthAttachment()
{
    Texture2DCreateInfo info{};
    info.width = mWidth;
    info.height = mHeight;
    info.dataType = GL_FLOAT;
    info.format = info.internalFormat = GL_DEPTH_COMPONENT;
    info.minFilter = info.magFilter = GL_NEAREST;
    info.wrapS = info.wrapT = GL_CLAMP_TO_BORDER;

    mDepthAttachment.create2DEmpty(info);

    mDepthAttachment.bind();
    // float border[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const vec4 border(1.0f);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border.x);
    mDepthAttachment.unbind();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthAttachment.GetHandle(), 0);
}

void RenderTarget::Create(int width, int height)
{
    mWidth = width;
    mHeight = height;
    glGenFramebuffers(1, &mHandle);
}

static std::vector<RenderTarget*> g_rts;

void DepthRenderTarget::Create(int width, int height)
{
    RenderTarget::Create(width, height);

    Bind();
    RenderTarget::CreateDepthAttachment();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    CheckError();
    Unbind();

    g_rts.push_back(this);
}

void RenderTarget::CheckError()
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("Frame buffer not completed");
    }
}

// TODO: expose
enum
{
    POSISION = 0,
    NORMAL = 1,
    ALBEDO = 2,
    FINALIMAGE = 0,
    SSAO = 0
};

void GBuffer::Create(int width, int height)
{
    RenderTarget::Create(width, height);

    mColorAttachmentCount = 3;

    Bind();

    Texture2DCreateInfo info{};
    info.width = width;
    info.height = height;
    info.dataType = GL_FLOAT;
    info.format = GL_RGBA;
    info.internalFormat = GL_RGBA16F;
    info.minFilter = info.magFilter = GL_NEAREST;

    // position
    mColorAttachments[POSISION].create2DEmpty(info);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + POSISION,
        GL_TEXTURE_2D,
        mColorAttachments[POSISION].GetHandle(),
        0);

    // normal
    mColorAttachments[NORMAL].create2DEmpty(info);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + NORMAL,
        GL_TEXTURE_2D,
        mColorAttachments[NORMAL].GetHandle(),
        0);

    // albedo
    info.internalFormat = GL_RGBA;
    info.dataType = GL_UNSIGNED_BYTE;
    mColorAttachments[ALBEDO].create2DEmpty(info);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + ALBEDO,
        GL_TEXTURE_2D,
        mColorAttachments[ALBEDO].GetHandle(),
        0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // depth
    CreateDepthAttachment();

    CheckError();
    Unbind();
}

void SsaoRT::Create(int width, int height)
{
    RenderTarget::Create(width, height);

    mColorAttachmentCount = 1;

    Bind();

    Texture2DCreateInfo info{};
    info.width = width;
    info.height = height;
    info.minFilter = info.magFilter = GL_NEAREST;

    // info.dataType       = GL_UNSIGNED_BYTE;
    // info.format         = GL_RGBA;
    // info.internalFormat = GL_RGBA;
    info.dataType = GL_FLOAT;
    info.format = GL_RED;
    info.internalFormat = GL_R32F;

    // position
    const int slot = SSAO;
    mColorAttachments[slot].create2DEmpty(info);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + slot,
        GL_TEXTURE_2D,
        mColorAttachments[slot].GetHandle(),
        0);

    GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    CheckError();
    Unbind();
}

void FinalImageRT::Create(int width, int height)
{
    RenderTarget::Create(width, height);

    mColorAttachmentCount = 1;
    Bind();

    Texture2DCreateInfo info{};
    info.width = width;
    info.height = height;
    info.minFilter = info.magFilter = GL_NEAREST;
    info.dataType = GL_UNSIGNED_BYTE;
    info.format = GL_RGBA;
    info.internalFormat = GL_RGBA;

    // position
    const int slot = FINALIMAGE;
    mColorAttachments[slot].create2DEmpty(info);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + slot,
        GL_TEXTURE_2D,
        mColorAttachments[slot].GetHandle(),
        0);

    GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    CheckError();
    Unbind();
}

DepthRenderTarget g_shadowRT;
GBuffer g_gbufferRT;
SsaoRT g_ssaoRT;
FinalImageRT g_finalImageRT;
FinalImageRT g_fxaaRT;

void R_CreateRT()
{
    const ivec2 extent = MainWindow::FrameSize();
    const int w = extent.x;
    const int h = extent.y;

    const int res = Dvar_GetInt(r_shadowRes);
    check(is_power_of_two(res));

    // g_shadowRT.Create( NUM_CASCADES * res, res );
    g_shadowRT.Create(res, res);
    g_gbufferRT.Create(w, h);
    g_ssaoRT.Create(w, h);
    g_finalImageRT.Create(w, h);
    g_fxaaRT.Create(w, h);
}

void R_DestroyRT()
{
    for (auto& rt : g_rts)
    {
        rt->Destroy();
    }
}