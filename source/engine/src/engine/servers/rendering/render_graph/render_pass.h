#pragma once
#include "../GLPrerequisites.h"
#include "core/collections/fixed_stack.h"

namespace vct::rg {

enum PixelFormat {
    FORMAT_R8_UINT,
    FORMAT_R8G8_UINT,
    FORMAT_R8G8B8_UINT,
    FORMAT_R8G8B8A8_UINT,

    FORMAT_R16_FLOAT,
    FORMAT_R16G16_FLOAT,
    FORMAT_R16G16B16_FLOAT,
    FORMAT_R16G16B16A16_FLOAT,

    FORMAT_R32_FLOAT,
    FORMAT_R32G32_FLOAT,
    FORMAT_R32G32B32_FLOAT,
    FORMAT_R32G32B32A32_FLOAT,

    FORMAT_D32_FLOAT,
};

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
