#pragma once
#include "graph.h"

namespace vct {

bool Graph::has_edge(int from, int to) const {
    return m_adj[from].find(to) != m_adj[from].end();
}

void Graph::add_edge(int from, int to) {
    DEV_ASSERT(from != to);
    DEV_ASSERT(!has_edge(from, to));
    m_adj[from].insert(to);
    return;
}

bool Graph::has_cycle() const {
    for (int i = 0; i < m_num_vertex; ++i) {
        for (int from : m_adj[i]) {
            if (is_reachable(from, i)) {
                return true;
            }
        }
    }
    return false;
}

int Graph::is_reachable(int from, int to) const {
    DEV_ASSERT(from != to);
    DEV_ASSERT_INDEX(from, m_num_vertex);
    DEV_ASSERT_INDEX(to, m_num_vertex);

    if (from == to) {
        return true;
    }

    std::queue<int> queue;
    std::vector<bool> visited(m_num_vertex, false);
    visited[from] = true;
    queue.push(from);

    while (!queue.empty()) {
        from = queue.front();
        queue.pop();

        for (int node : m_adj[from]) {
            if (node == to) {
                return true;
            }

            if (!visited[node]) {
                visited[node] = true;
                queue.push(node);
            }
        }
    }

    return false;
}

void Graph::remove_redundant() {
    for (int from = 0; from < m_num_vertex; ++from) {
        for (int to = 0; to < m_num_vertex; ++to) {
            if (from == to) {
                continue;
            }

            auto& adj = m_adj[from];
            bool has_direct_path = adj.find(to) != adj.end();
            if (!has_direct_path) {
                continue;
            }

            for (int middle = 0; middle < m_num_vertex; ++middle) {
                if (middle == from || middle == to) {
                    continue;
                }

                if (is_reachable(from, middle) && is_reachable(middle, to)) {
                    adj.erase(to);
                }
            }
        }
    }
}

std::vector<int> Graph::sort() const {
    std::vector<int> sorted;
    sorted.reserve(m_num_vertex);
    for (int i = 0; i < m_num_vertex; ++i) {
        sorted.emplace_back(i);
    }

    std::sort(sorted.begin(), sorted.end(), [this](const int& a, const int& b) {
        return is_reachable(a, b);
    });
    return sorted;
}

std::vector<std::vector<int>> Graph::build_level() const {
    std::vector<std::vector<int>> levels(1);
    std::list<int> list;
    for (int i = 0; i < m_num_vertex; ++i) {
        bool no_dependency = true;
        for (int j = 0; j < m_num_vertex; ++j) {
            if (i == j) {
                continue;
            }
            if (has_edge(j, i)) {
                no_dependency = false;
                break;
            }
        }
        if (no_dependency) {
            levels[0].push_back(i);
        } else {
            list.push_back(i);
        }
    }

    while (!list.empty()) {
        std::vector<int> level;
        auto it = list.begin();
        while (it != list.end()) {
            bool exist_edge = false;
            for (int i : levels.back()) {
                if (has_edge(i, *it)) {
                    exist_edge = true;
                    break;
                }
            }
            if (exist_edge) {
                level.push_back(*it);
                it = list.erase(it);
            } else {
                ++it;
            }
        }
        levels.emplace_back(level);
    }

    return levels;
}

}  // namespace vct
