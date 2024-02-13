#include "engine/core/base/intrusive_list.h"

namespace vct {

class MyList : public IntrusiveList<int> {
public:
    std::string to_string() const {
        std::string result;
        for (Node* cursor = m_head; cursor; cursor = cursor->next) {
            result.append(std::to_string(cursor->data));
            result.push_back(';');
        }

        return result;
    }
};

TEST(intrusive_list, empty) {
    MyList list;
    EXPECT_TRUE(list.empty());
}

TEST(intrusive_list, node_push_back) {
    MyList list;
    MyList::Node node1(1);
    MyList::Node node2(2);
    MyList::Node node3(3);

    list.node_push_back(&node1);
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.to_string(), "1;");
    list.node_push_back(&node2);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.to_string(), "1;2;");
    list.node_push_back(&node3);
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.to_string(), "1;2;3;");
}

TEST(intrusive_list, node_push_front) {
    MyList list;
    MyList::Node node1(1);
    MyList::Node node2(2);
    MyList::Node node3(3);

    list.node_push_front(&node1);
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.to_string(), "1;");
    list.node_push_front(&node2);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.to_string(), "2;1;");
    list.node_push_front(&node3);
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.to_string(), "3;2;1;");
}

TEST(intrusive_list, node_pop_front) {
    MyList list;
    MyList::Node node1(1);
    MyList::Node node2(2);
    MyList::Node node3(3);

    list.node_push_front(&node1);
    list.node_push_front(&node2);
    list.node_push_front(&node3);

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.to_string(), "3;2;1;");
    EXPECT_EQ(list.node_front(), &node3);
    EXPECT_EQ(list.node_back(), &node1);

    auto front = list.node_pop_front();
    EXPECT_EQ(front->data, node3.data);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.to_string(), "2;1;");
    EXPECT_EQ(list.node_front(), &node2);
    EXPECT_EQ(list.node_back(), &node1);

    front = list.node_pop_front();
    EXPECT_EQ(front->data, node2.data);
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.to_string(), "1;");
    EXPECT_EQ(list.node_front(), &node1);
    EXPECT_EQ(list.node_back(), &node1);

    front = list.node_pop_front();
    EXPECT_EQ(front->data, node1.data);
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.node_front(), nullptr);
    EXPECT_EQ(list.node_back(), nullptr);

    front = list.node_pop_front();
    EXPECT_EQ(front, nullptr);
    EXPECT_TRUE(list.empty());
}

TEST(intrusive_list, node_remove) {
    MyList list;
    MyList::Node node1(1);
    MyList::Node node2(2);
    MyList::Node node3(3);
    MyList::Node node4(4);
    MyList::Node node5(5);

    list.node_push_front(&node1);
    list.node_push_front(&node2);
    list.node_push_front(&node3);
    list.node_push_front(&node4);
    list.node_push_front(&node5);
    EXPECT_EQ(list.size(), 5);
    EXPECT_EQ(list.to_string(), "5;4;3;2;1;");

    bool removed = list.node_remove(&node3);
    EXPECT_EQ(removed, true);
    EXPECT_EQ(list.size(), 4);
    EXPECT_EQ(list.to_string(), "5;4;2;1;");
    removed = list.node_remove(&node3);
    EXPECT_EQ(removed, false);
    EXPECT_EQ(list.size(), 4);
    EXPECT_EQ(list.to_string(), "5;4;2;1;");
    removed = list.node_remove(&node5);
    EXPECT_EQ(removed, true);
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.to_string(), "4;2;1;");

    list.node_push_front(&node3);
    list.node_push_back(&node5);
    EXPECT_EQ(list.size(), 5);
    EXPECT_EQ(list.to_string(), "3;4;2;1;5;");

    list.node_remove(&node1);
    list.node_remove(&node5);
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.to_string(), "3;4;2;");
}

TEST(intrusive_list, iterator) {
    MyList list;
    MyList::Node node1(1);
    MyList::Node node2(2);
    MyList::Node node3(3);
    MyList::Node node4(4);
    MyList::Node node5(5);

    list.node_push_front(&node1);
    list.node_push_front(&node2);
    list.node_push_front(&node3);
    list.node_push_front(&node4);
    list.node_push_front(&node5);
    EXPECT_EQ(list.size(), 5);
    EXPECT_EQ(list.to_string(), "5;4;3;2;1;");

    int i = 5;
    for (int iter : list) {
        EXPECT_EQ(iter, i);
        --i;
    }

    list.node_remove(&node1);
    list.node_remove(&node2);
    list.node_remove(&node3);
    list.node_remove(&node4);
    list.node_remove(&node5);
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.to_string(), "");

    list.node_push_back(&node1);
    list.node_push_back(&node2);
    list.node_push_back(&node3);
    list.node_push_back(&node4);
    list.node_push_back(&node5);
    EXPECT_EQ(list.size(), 5);
    EXPECT_EQ(list.to_string(), "1;2;3;4;5;");

    i = 1;
    for (int iter : list) {
        EXPECT_EQ(iter, i);
        ++i;
    }
}

}  // namespace vct
