#pragma once

namespace vct {

class Graph {
public:
    Graph(int num_vertex) : m_num_vertex(num_vertex) {
        m_adj.resize(num_vertex);
    }

    bool has_edge(int from, int to) const;

    void add_edge(int from, int to);

    bool has_cycle() const;

    int is_reachable(int from, int to) const;

    void remove_redundant();

    std::vector<int> sort() const;

    std::vector<std::vector<int>> build_level() const;

private:
    std::vector<int> m_nodes;
    std::vector<std::unordered_set<int>> m_adj;
    const int m_num_vertex;
};

}  // namespace vct
