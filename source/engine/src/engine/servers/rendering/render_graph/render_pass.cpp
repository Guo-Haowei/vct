#include "render_pass.h"

#include "../GLPrerequisites.h"

namespace vct::rg {

void RenderPass::execute() {
    bind();

    m_pass_desc->func();

    unbind();
}

void RenderPass::create(const RenderPassDesc& pass_desc, int w, int h) {
    m_pass_desc = &pass_desc;
    m_width = w;
    m_height = h;
}

void RenderPassGL::create(const RenderPassDesc& pass_desc, int w, int h) {
    RenderPass::create(pass_desc, w, h);

    glGenFramebuffers(1, &m_fbo_handle);
    bind();

    // create color attachments
    uint32_t color_attachment_count = static_cast<uint32_t>(pass_desc.color_attachments.size());
    DEV_ASSERT(color_attachment_count <= m_color_attachments.capacity());

    m_color_attachments.resize(color_attachment_count);

    std::vector<GLuint> attachments;
    attachments.reserve(color_attachment_count);
    glGenTextures(color_attachment_count, m_color_attachments.data());
    for (uint32_t i = 0; i < color_attachment_count; ++i) {
        const auto& desc = pass_desc.color_attachments[i];

        // create texture
        GLuint color_attachment_id = m_color_attachments[i];
        glBindTexture(GL_TEXTURE_2D, color_attachment_id);
        glTexImage2D(GL_TEXTURE_2D,                              // target
                     0,                                          // level
                     format_to_gl_internal_format(desc.format),  // internal format
                     m_width,                                    // width
                     m_height,                                   // height
                     0,                                          // boarder
                     format_to_gl_format(desc.format),           // format
                     format_to_gl_data_type(desc.format),        // type
                     nullptr                                     // pixels
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);

        // bind to frame buffer
        GLuint attachment = GL_COLOR_ATTACHMENT0 + i;
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,       // target
            attachment,           // attachment
            GL_TEXTURE_2D,        // texture target
            color_attachment_id,  // texture
            0                     // level
        );

        attachments.push_back(attachment);
    }

    if (attachments.empty()) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    } else {
        glDrawBuffers(static_cast<uint32_t>(attachments.size()), attachments.data());
    }

    // create depth attachments
    if (pass_desc.depth_attachment.has_value()) {
        const auto& desc = *pass_desc.depth_attachment;
        glGenTextures(1, &m_depth_attachment);
        glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
        glTexImage2D(GL_TEXTURE_2D,                              // target
                     0,                                          // level
                     format_to_gl_internal_format(desc.format),  // internal format
                     m_width,                                    // width
                     m_height,                                   // height
                     0,                                          // boarder
                     format_to_gl_format(desc.format),           // format
                     format_to_gl_data_type(desc.format),        // type
                     nullptr                                     // pixels
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

        glBindTexture(GL_TEXTURE_2D, 0);

        // bind to frame buffer
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,       // target
            GL_DEPTH_ATTACHMENT,  // attachment
            GL_TEXTURE_2D,        // texture target
            m_depth_attachment,   // texture
            0                     // level
        );
    }

    DEV_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    unbind();
}

void RenderPassGL::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_handle);
}

void RenderPassGL::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t RenderPassGL::get_color_attachment(int index) {
    DEV_ASSERT_INDEX(index, m_color_attachments.size());
    return m_color_attachments[index];
}

uint32_t RenderPassGL::get_depth_attachment() {
    DEV_ASSERT(m_depth_attachment);
    return m_depth_attachment;
}

}  // namespace vct::rg
