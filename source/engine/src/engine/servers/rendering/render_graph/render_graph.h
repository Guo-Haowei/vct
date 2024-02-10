#pragma once
#include "render_pass.h"

namespace vct {

class RenderGraph {
public:
    void add_pass(RenderPassDesc& desc);

    std::shared_ptr<RenderPass> find_pass(const std::string& name);

    void compile();

    void execute();

public:
    std::vector<std::shared_ptr<RenderPass>> m_render_passes;
    std::vector<int> m_sorted_order;
    std::vector<std::pair<int, int>> m_links;

private:
    std::map<std::string, int> m_render_pass_lookup;
};

}  // namespace vct
