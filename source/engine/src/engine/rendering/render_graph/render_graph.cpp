#include "render_graph.h"

namespace vct {

void RenderGraph::add_pass(RenderPassDesc& desc) {
    std::shared_ptr<RenderPass> render_pass = std::make_shared<RenderPassGL>();
    render_pass->create_internal(desc);
    m_render_passes.emplace_back(render_pass);

    const std::string& name = render_pass->m_name;
    DEV_ASSERT(m_render_pass_lookup.find(name) == m_render_pass_lookup.end());
    m_render_pass_lookup[name] = (int)m_render_passes.size() - 1;
}

std::shared_ptr<RenderPass> RenderGraph::find_pass(const std::string& name) {
    auto it = m_render_pass_lookup.find(name);
    if (it == m_render_pass_lookup.end()) {
        return nullptr;
    }

    return m_render_passes[it->second];
}

void RenderGraph::compile() {
    const int num_passes = (int)m_render_passes.size();

    Graph graph(num_passes);

    for (int pass_index = 0; pass_index < num_passes; ++pass_index) {
        const std::shared_ptr<RenderPass>& pass = m_render_passes[pass_index];
        for (const std::string& input : pass->m_inputs) {
            auto it = m_render_pass_lookup.find(input);
            if (it == m_render_pass_lookup.end()) {
                CRASH_NOW_MSG(std::format("dependency '{}' not found", input));
            } else {
                graph.add_edge(it->second, pass_index);
            }
        }
    }

    if (graph.has_cycle()) {
        CRASH_NOW_MSG("render graph has cycle");
    }

    graph.remove_redundant();

    m_levels = graph.build_level();
    m_sorted_order.reserve(num_passes);
    for (const auto& level : m_levels) {
        for (int i : level) {
            m_sorted_order.emplace_back(i);
        }
    }

    for (int i = 1; i < (int)m_levels.size(); ++i) {
        for (int from : m_levels[i - 1]) {
            for (int to : m_levels[i]) {
                if (graph.has_edge(from, to)) {
                    const RenderPass* a = m_render_passes[from].get();
                    const RenderPass* b = m_render_passes[to].get();
                    LOG_VERBOSE("[render graph] dependency from '{}' to '{}'", a->get_name(), b->get_name());
                    m_links.push_back({ from, to });
                }
            }
        }
    }
}

void RenderGraph::execute() {
    for (int index : m_sorted_order) {
        m_render_passes[index]->execute();
    }
}

}  // namespace vct
