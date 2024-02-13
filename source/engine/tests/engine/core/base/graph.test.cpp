#include "core/base/graph.h"

namespace vct {

TEST(Graph, has_cycle) {
    Graph graph(5);

    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);
    graph.add_edge(3, 4);
    EXPECT_FALSE(graph.has_cycle());
    // introduce a cycle
    graph.add_edge(4, 1);
    EXPECT_TRUE(graph.has_cycle());
}

TEST(Graph, sort) {
    Graph graph(5);

    enum {
        A = 2,
        B = 1,
        C = 4,
        D = 0,
        E = 3,
    };

    graph.add_edge(A, B);  // A -> B
    graph.add_edge(B, C);  // B -> C
    graph.add_edge(C, D);  // C -> D
    graph.add_edge(D, E);  // D -> E

    EXPECT_FALSE(graph.has_cycle());
    auto sorted = graph.sort();

    EXPECT_EQ(sorted[0], A);
    EXPECT_EQ(sorted[1], B);
    EXPECT_EQ(sorted[2], C);
    EXPECT_EQ(sorted[3], D);
    EXPECT_EQ(sorted[4], E);
}

TEST(Graph, remove_redundant) {
    Graph graph(5);

    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);
    graph.add_edge(3, 4);
    graph.add_edge(1, 3);
    graph.add_edge(0, 4);

    EXPECT_FALSE(graph.has_cycle());

    EXPECT_TRUE(graph.has_edge(0, 1));
    EXPECT_TRUE(graph.has_edge(1, 2));
    EXPECT_TRUE(graph.has_edge(2, 3));
    EXPECT_TRUE(graph.has_edge(3, 4));
    EXPECT_TRUE(graph.has_edge(1, 3));
    EXPECT_TRUE(graph.has_edge(0, 4));

    graph.remove_redundant();

    EXPECT_TRUE(graph.has_edge(0, 1));
    EXPECT_TRUE(graph.has_edge(1, 2));
    EXPECT_TRUE(graph.has_edge(2, 3));
    EXPECT_TRUE(graph.has_edge(3, 4));
    EXPECT_FALSE(graph.has_edge(1, 3));
    EXPECT_FALSE(graph.has_edge(0, 4));
}

TEST(Graph, sort_level) {
    enum {
        A = 0,
        B = 1,
        C = 2,
        D = 3,
        E = 4,
        F = 5,
    };

    Graph graph(6);

    graph.add_edge(A, B);  // A -> B
    graph.add_edge(C, D);  // C -> D
    graph.add_edge(B, E);  // B -> E
    graph.add_edge(D, E);  // D -> E
    graph.add_edge(E, F);  // E -> F
    /*
    A -> B \
             E -> F
    C -> D /
    */
    EXPECT_FALSE(graph.has_cycle());

    auto levels = graph.build_level();

    EXPECT_EQ(levels[0][0], A);
    EXPECT_EQ(levels[0][1], C);
    EXPECT_EQ(levels[1][0], B);
    EXPECT_EQ(levels[1][1], D);
    EXPECT_EQ(levels[2][0], E);
    EXPECT_EQ(levels[3][0], F);
    return;
}

}  // namespace vct
