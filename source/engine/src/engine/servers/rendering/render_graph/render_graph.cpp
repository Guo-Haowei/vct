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
    const int pass_count = (int)m_render_passes.size();

    std::map<std::string, int> m_output_owner;
    for (int pass_index = 0; pass_index < pass_count; ++pass_index) {
        const std::shared_ptr<RenderPass>& pass = m_render_passes[pass_index];
        for (const std::string& output : pass->m_outputs) {
            DEV_ASSERT(m_output_owner.find(output) == m_output_owner.end());
            m_output_owner[output] = pass_index;
        }
    }

    const size_t alloc_size = sizeof(bool) * pass_count * pass_count;
    bool* dependencies = (bool*)malloc(alloc_size);
    memset(dependencies, 0, alloc_size);

    for (int pass_index = 0; pass_index < pass_count; ++pass_index) {
        const std::shared_ptr<RenderPass>& pass = m_render_passes[pass_index];
        for (const std::string& input : pass->m_inputs) {
            auto it = m_output_owner.find(input);
            if (it == m_output_owner.end()) {
                CRASH_NOW_MSG(std::format("dependency '{}' not found", input));
            } else {
                dependencies[pass_index * pass_count + it->second] = true;
            }
        }
    }

    // @TODO: check for circular dependency

    std::vector<int> order;
    for (int i = 0; i < pass_count; ++i) {
        order.push_back(i);
    }

    // @TODO: remove redundant dependencies

    // sort
    while (true) {
        // @TODO: refactor
        bool all_sorted = true;
        for (int index : order) {
            if (index != -1) {
                all_sorted = false;
            }
        }
        if (all_sorted) {
            break;
        }

        for (int pass_index : order) {
            if (pass_index == -1) {
                continue;
            }

            bool has_dependency = false;
            for (size_t dependency_index = 0; dependency_index < pass_count; ++dependency_index) {
                bool sorted = std::find(m_sorted_order.begin(), m_sorted_order.end(), dependency_index) != m_sorted_order.end();
                if (sorted) {
                    continue;
                }
                if (dependencies[pass_index * pass_count + dependency_index]) {
                    has_dependency = true;
                    break;
                }
            }

            if (has_dependency) {
                continue;
            }

            order[pass_index] = -1;
            m_sorted_order.push_back(pass_index);
            break;
        }
    }

    // links
    for (int pass_index = 0; pass_index < pass_count; ++pass_index) {
        for (int dependency_index = 0; dependency_index < pass_count; ++dependency_index) {
            if (dependencies[pass_index * pass_count + dependency_index]) {
                m_links.emplace_back(std::make_pair(dependency_index, pass_index));
            }
        }
    }

    free(dependencies);
}

void RenderGraph::execute() {
    for (int index : m_sorted_order) {
        m_render_passes[index]->execute();
    }
}

}  // namespace vct
