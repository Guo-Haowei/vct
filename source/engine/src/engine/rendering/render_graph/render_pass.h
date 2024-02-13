#pragma once
#include "assets/image.h"
#include "core/base/fixed_stack.h"

namespace vct {

struct RenderTargetDesc {
    std::string name;
    PixelFormat format;
};

using RenderPassFunc = void (*)();

enum RenderPassType {
    RENDER_PASS_SHADING,
    RENDER_PASS_COMPUTE,
};

struct RenderPassDesc {
    RenderPassType type = RENDER_PASS_SHADING;
    std::string name;
    std::vector<std::string> dependencies;
    std::vector<RenderTargetDesc> color_attachments;
    std::optional<RenderTargetDesc> depth_attachment;
    RenderPassFunc func = nullptr;
    int width;
    int height;
};

class RenderPass {
public:
    virtual ~RenderPass() = default;

    void execute();

    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual uint32_t get_color_attachment(int index) = 0;
    virtual uint32_t get_depth_attachment() = 0;

    const std::string& get_name() const { return m_name; }

protected:
    virtual void create_internal(RenderPassDesc& pass_desc);

    std::string m_name;
    std::vector<std::string> m_inputs;
    std::vector<std::string> m_outputs;
    RenderPassFunc m_func;

    int m_width = 0;
    int m_height = 0;

    friend class RenderGraph;
};

class RenderPassGL : public RenderPass {
public:
    void bind() override;
    void unbind() override;

    uint32_t get_color_attachment(int index) override;
    uint32_t get_depth_attachment() override;

protected:
    void create_internal(RenderPassDesc& pass_desc) override;

    FixedStack<uint32_t, 8> m_color_attachments;
    uint32_t m_depth_attachment = 0;
    uint32_t m_fbo_handle = 0;
};

}  // namespace vct
