#pragma once
#include "assets/image.h"
#include "core/collections/fixed_stack.h"

namespace vct::rg {

struct RenderTargetDesc {
    std::string name;
    PixelFormat format;
};

using RenderPassFunc = void (*)();

struct RenderPassDesc {
    std::string name;
    std::vector<RenderTargetDesc> color_attachments;
    std::optional<RenderTargetDesc> depth_attachment;
    RenderPassFunc func;
};

class RenderPass {
public:
    virtual ~RenderPass() = default;
    virtual void create(const RenderPassDesc& pass_desc, int w, int h);

    void execute();

    virtual void bind() = 0;
    virtual void unbind() = 0;

protected:
    const RenderPassDesc* m_pass_desc;
    int m_width = 0;
    int m_height = 0;
};

class RenderPassGL : public RenderPass {
public:
    virtual void create(const RenderPassDesc& pass_desc, int w, int h) override;

    virtual void bind() override;
    virtual void unbind() override;

    uint32_t get_color_attachment(int index);
    uint32_t get_depth_attachment();

private:
    FixedStack<uint32_t, 8> m_color_attachments;
    uint32_t m_depth_attachment = 0;
    uint32_t m_fbo_handle = 0;
};

}  // namespace vct::rg
