#include "render_pass.h"

#include "rendering/GLPrerequisites.h"

namespace vct {

void RenderPass::execute() {
    bind();

    if (m_func) {
        m_func();
    }

    unbind();
}

void RenderPass::create_internal(RenderPassDesc& desc) {
    m_name = std::move(desc.name);
    m_inputs = std::move(desc.dependencies);
    for (const auto& output : desc.color_attachments) {
        m_outputs.emplace_back(output.name);
    }
    if (desc.depth_attachment) {
        m_outputs.emplace_back(desc.depth_attachment->name);
    }
    m_func = desc.func;
    m_width = desc.width;
    m_height = desc.height;
}

void RenderPassGL::create_internal(RenderPassDesc& desc) {
    RenderPass::create_internal(desc);

    if (desc.type == RENDER_PASS_COMPUTE) {
        return;
    }

    glGenFramebuffers(1, &m_fbo_handle);
    bind();

    // create color attachments
    uint32_t color_attachment_count = static_cast<uint32_t>(desc.color_attachments.size());
    DEV_ASSERT(color_attachment_count <= m_color_attachments.capacity());

    m_color_attachments.resize(color_attachment_count);

    std::vector<GLuint> attachments;
    attachments.reserve(color_attachment_count);
    glGenTextures(color_attachment_count, m_color_attachments.data());
    for (uint32_t i = 0; i < color_attachment_count; ++i) {
        const auto& color_attachment = desc.color_attachments[i];

        // create texture
        GLuint color_attachment_id = m_color_attachments[i];
        glBindTexture(GL_TEXTURE_2D, color_attachment_id);
        glTexImage2D(GL_TEXTURE_2D,                                          // target
                     0,                                                      // level
                     format_to_gl_internal_format(color_attachment.format),  // internal format
                     m_width,                                                // width
                     m_height,                                               // height
                     0,                                                      // boarder
                     format_to_gl_format(color_attachment.format),           // format
                     format_to_gl_data_type(color_attachment.format),        // type
                     nullptr                                                 // pixels
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
    if (desc.depth_attachment.has_value()) {
        const auto& depth_attachment = *desc.depth_attachment;
        glGenTextures(1, &m_depth_attachment);
        glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
        glTexImage2D(GL_TEXTURE_2D,                                          // target
                     0,                                                      // level
                     format_to_gl_internal_format(depth_attachment.format),  // internal format
                     m_width,                                                // width
                     m_height,                                               // height
                     0,                                                      // boarder
                     format_to_gl_format(depth_attachment.format),           // format
                     format_to_gl_data_type(depth_attachment.format),        // type
                     nullptr                                                 // pixels
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

}  // namespace vct
