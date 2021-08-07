#pragma once
#include "GpuResource.h"
#include "GpuTexture.h"

class RenderTarget : public GpuResource {
   public:
    enum { MAX_COLOR_ATTACHMENT = 4 };

    virtual void create( int width, int height );
    void bind();
    void unbind();
    void destroy();
    void createDepthAttachment();
    void checkError();

    const GpuTexture& getDepthTexture() const { return m_depthAttachment; }
    const GpuTexture& getColorAttachment( int i ) const { return m_colorAttachments[i]; }

   protected:
    GpuTexture m_depthAttachment;
    GpuTexture m_colorAttachments[MAX_COLOR_ATTACHMENT];
    int m_colorAttachmentCount = 0;
    int m_width                = 0;
    int m_height               = 0;
};

class DepthRenderTarget : public RenderTarget {
   public:
    virtual void create( int width, int height ) override;
};

class GBuffer : public RenderTarget {
   public:
    virtual void create( int width, int height ) override;
};
