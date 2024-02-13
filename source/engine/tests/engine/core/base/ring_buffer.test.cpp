#include "engine/core/base/ring_buffer.h"

namespace vct {

TEST(ring_buffer, constructor) {
    RingBuffer<int, 4> rb;
    EXPECT_TRUE(rb.empty());
    rb.push_back(2);
    EXPECT_FALSE(rb.empty());
    rb.clear();
    EXPECT_TRUE(rb.empty());
}

TEST(ring_buffer, constructor_initializer_list) {
    RingBuffer<int, 6> rb{ 1, 2, 3 };
    EXPECT_FALSE(rb.empty());
    EXPECT_EQ(rb.size(), 3);
    EXPECT_EQ(rb[0], 1);
    EXPECT_EQ(rb[1], 2);
    EXPECT_EQ(rb[2], 3);
}

TEST(ring_buffer, emtpy) {
    {
        RingBuffer<int, 4> rb;
        EXPECT_TRUE(rb.empty());
    }
    {
        RingBuffer<int, 4> rb{ 1 };
        EXPECT_FALSE(rb.empty());
    }
    {
        RingBuffer<int, 4> rb{ 1, 2 };
        EXPECT_FALSE(rb.empty());
    }
}

TEST(ring_buffer, size) {
    RingBuffer<char, 4> rb;
    EXPECT_EQ(rb.size(), 0);
    rb.push_back('a');
    rb.push_back('b');
    EXPECT_EQ(rb.size(), 2);
    rb.push_back('c');
    EXPECT_EQ(rb.size(), 3);
    rb.push_back('d');
    EXPECT_EQ(rb.size(), 4);
    rb.push_back('e');
    EXPECT_EQ(rb.size(), 4);
    rb.pop_front();
    EXPECT_EQ(rb.size(), 3);
    rb.pop_front();
    EXPECT_EQ(rb.size(), 2);
    rb.pop_front();
    EXPECT_EQ(rb.size(), 1);
    rb.pop_front();
    EXPECT_EQ(rb.size(), 0);
}

TEST(ring_buffer, capacity) {
    {
        RingBuffer<int, 1> rb;
        static_assert(rb.capacity() == 1);
    }
    {
        RingBuffer<int, 100> rb;
        static_assert(rb.capacity() == 100);
    }
}

TEST(ring_buffer, push_back) {
    RingBuffer<int, 3> rb;
    rb.push_back(5);
    EXPECT_EQ(rb.front(), 5);
    EXPECT_EQ(rb.back(), 5);
    rb.push_back(6);
    EXPECT_EQ(rb.front(), 5);
    EXPECT_EQ(rb.back(), 6);
    rb.push_back(7);
    EXPECT_EQ(rb.front(), 5);
    EXPECT_EQ(rb.back(), 7);
    rb.push_back(8);
    EXPECT_EQ(rb.front(), 6);
    EXPECT_EQ(rb.back(), 8);
}

TEST(ring_buffer, pop_front) {
    RingBuffer<int, 3> rb;
    rb.push_back(5);
    rb.push_back(6);
    rb.push_back(7);
    EXPECT_EQ(rb.front(), 5);
    EXPECT_EQ(rb.back(), 7);
    rb.pop_front();
    EXPECT_EQ(rb.front(), 6);
    EXPECT_EQ(rb.back(), 7);
    rb.pop_front();
    EXPECT_EQ(rb.front(), 7);
    EXPECT_EQ(rb.back(), 7);
    rb.pop_front();
    EXPECT_TRUE(rb.empty());
}

TEST(ring_buffer, ring_buffer_of_size_1) {
    RingBuffer<int, 1> rb;
    EXPECT_EQ(rb.size(), 0);
    EXPECT_EQ(rb.capacity(), 1);
    EXPECT_TRUE(rb.empty());

    rb.push_back(15);
    EXPECT_EQ(rb.size(), 1);
    EXPECT_EQ(rb[0], 15);
    EXPECT_EQ(rb.front(), 15);
    EXPECT_EQ(rb.back(), 15);

    rb.push_back(2);
    EXPECT_EQ(rb.size(), 1);
    EXPECT_EQ(rb[0], 2);
    EXPECT_EQ(rb.front(), 2);
    EXPECT_EQ(rb.back(), 2);
}

TEST(ring_buffer, ring_buffer_of_size_3) {
    RingBuffer<int, 3> rb;
    EXPECT_EQ(rb.size(), 0);
    EXPECT_EQ(rb.capacity(), 3);
    EXPECT_TRUE(rb.empty());

    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    EXPECT_EQ(rb.size(), 3);
    EXPECT_EQ(rb[0], 1);
    EXPECT_EQ(rb[1], 2);
    EXPECT_EQ(rb[2], 3);
    EXPECT_EQ(rb.front(), 1);
    EXPECT_EQ(rb.back(), 3);
}

TEST(ring_buffer, ring_buffer_of_size_4) {
    constexpr int capacity = 4;
    RingBuffer<int, capacity> rb;
    EXPECT_EQ(rb.size(), 0);
    EXPECT_EQ(rb.capacity(), capacity);
    EXPECT_TRUE(rb.empty());

    for (int i = 0; i < capacity; ++i) {
        rb.push_back(i);
    }
    for (int& v : rb) {
        ++v;
    }
    for (int i = 0; i < capacity; ++i) {
        EXPECT_EQ(rb[i], i + 1);
    }
    // 1, 2, 3, 4
    rb.pop_front();
    EXPECT_EQ(rb.size(), capacity - 1);
    EXPECT_EQ(rb.front(), 2);
    EXPECT_EQ(rb.back(), 4);
    // 2, 3, 4
    rb.push_back(10);
    rb.push_back(20);
    // 3, 4, 10, 20
    EXPECT_EQ(rb[0], 3);
    EXPECT_EQ(rb[1], 4);
    EXPECT_EQ(rb[2], 10);
    EXPECT_EQ(rb[3], 20);
}

TEST(ring_buffer, iterator) {
    RingBuffer<int, 5> stack{ 0, 1, 2, 3, 4 };  // 2, 3, 4
    int i = 0;
    for (auto& it : stack) {
        EXPECT_EQ(it, i);
        ++i;
    }
}

TEST(ring_buffer, const_iterator) {
    RingBuffer<int, 4> stack{ 0, 1, 2, 3, 4 };  // 1, 2, 3, 4
    int i = 1;
    for (const auto& it : stack) {
        EXPECT_EQ(it, i);
        ++i;
    }
}

TEST(ring_buffer, reverse_iterator) {
    RingBuffer<int, 7> stack{ 0, 1, 2, 3, 4 };  // 1, 2, 3, 4
    int i = 4;
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        EXPECT_EQ(*it, i);
        --i;
    }
}

TEST(ring_buffer, reverse_const_iterator) {
    RingBuffer<int, 5> stack{ 0, 1, 2, 3, 4 };  // 2, 3, 4
    int i = 4;
    for (auto it = stack.crbegin(); it != stack.crend(); ++it) {
        EXPECT_EQ(*it, i);
        --i;
    }
}

}  // namespace vct
