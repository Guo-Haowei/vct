#include "core/collections/fixed_stack.h"

namespace vct {

TEST(FixedStack, constructor) {
    FixedStack<int, 7> stack;
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
    EXPECT_EQ(stack.capacity(), 7);
}

TEST(FixedStack, constructor_initializer_list) {
    FixedStack<int, 6> stack{ 1, 2, 3 };
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 3);
    EXPECT_EQ(stack[0], 1);
    EXPECT_EQ(stack[1], 2);
    EXPECT_EQ(stack[2], 3);
}

TEST(FixedStack, constructor_initializer_list_over_flow) {
    auto func = []() { [[maybe_unused]] FixedStack<int, 2> stack{ 1, 2, 3 }; };

    EXPECT_DEATH({ func(); }, "index out of range");
}

TEST(FixedStack, emtpy) {
    {
        FixedStack<int, 4> stack;
        EXPECT_TRUE(stack.empty());
    }
    {
        FixedStack<int, 4> stack{ 1 };
        EXPECT_FALSE(stack.empty());
    }
    {
        FixedStack<int, 4> stack{ 1, 2 };
        EXPECT_FALSE(stack.empty());
    }
}

TEST(FixedStack, size) {
    FixedStack<char, 4> stack;
    EXPECT_EQ(stack.size(), 0);
    stack.push_back('a');
    stack.push_back('b');
    EXPECT_EQ(stack.size(), 2);
    stack.push_back('c');
    EXPECT_EQ(stack.size(), 3);
    stack.push_back('d');
    EXPECT_EQ(stack.size(), 4);
    stack.pop_back();
    EXPECT_EQ(stack.size(), 3);
    stack.pop_back();
    EXPECT_EQ(stack.size(), 2);
    stack.pop_back();
    EXPECT_EQ(stack.size(), 1);
    stack.pop_back();
    EXPECT_EQ(stack.size(), 0);
}

TEST(FixedStack, capacity) {
    {
        FixedStack<int, 1> stack;
        static_assert(stack.capacity() == 1);
    }
    {
        FixedStack<int, 100> stack;
        static_assert(stack.capacity() == 100);
    }
}

TEST(FixedStack, push_back) {
    FixedStack<int, 5> stack{ 1, 2, 3 };
    stack.push_back(10);
    EXPECT_EQ(stack[3], 10);
    stack.push_back(11);
    EXPECT_EQ(stack[4], 11);

    EXPECT_DEATH({ stack.push_back(12); }, "index out of range");
}

TEST(FixedStack, pop_back) {
    FixedStack<int, 5> stack{ 1, 2, 3 };
    EXPECT_EQ(stack.back(), 3);
    stack.pop_back();
    EXPECT_EQ(stack.back(), 2);
    stack.pop_back();
    EXPECT_EQ(stack.back(), 1);
    stack.pop_back();

    EXPECT_DEATH({ stack.pop_back(); }, "index out of range");
}

TEST(FixedStack, front_and_back) {
    FixedStack<int, 5> stack;
    EXPECT_DEATH({ stack.front(); }, "index out of range");
    EXPECT_DEATH({ stack.back(); }, "index out of range");
    stack.push_back(10);
    EXPECT_EQ(stack.front(), 10);
    EXPECT_EQ(stack.back(), 10);
    stack.push_back(20);
    EXPECT_EQ(stack.front(), 10);
    EXPECT_EQ(stack.back(), 20);
}

TEST(FixedStack, iterator) {
    FixedStack<int, 5> stack{ 0, 1, 2, 3, 4 };
    int i = 0;
    for (auto& it : stack) {
        EXPECT_EQ(it, i);
        ++i;
    }
}

TEST(FixedStack, const_iterator) {
    FixedStack<int, 5> stack{ 0, 1, 2, 3, 4 };
    int i = 0;
    for (const auto& it : stack) {
        EXPECT_EQ(it, i);
        ++i;
    }
}

TEST(FixedStack, reverse_iterator) {
    FixedStack<int, 5> stack{ 0, 1, 2, 3, 4 };
    int i = 4;
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        EXPECT_EQ(*it, i);
        --i;
    }
}

TEST(FixedStack, reverse_const_iterator) {
    FixedStack<int, 5> stack{ 0, 1, 2, 3, 4 };
    int i = 4;
    for (auto it = stack.crbegin(); it != stack.crend(); ++it) {
        EXPECT_EQ(*it, i);
        --i;
    }
}

}  // namespace vct
