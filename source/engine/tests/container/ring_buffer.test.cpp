#include <engine/container/ring_buffer.h>

TEST(ring_buffer, ring_buffer_size_1)
{
    constexpr size_t capacity = 1;
    ring_buffer<int, capacity> buffer;
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.capacity(), capacity);
    EXPECT_TRUE(buffer.empty());
    {
        const int value = 1;
        buffer.push_back(value);
        EXPECT_EQ(buffer.size(), 1);
        EXPECT_EQ(buffer.at(0), value);
        EXPECT_EQ(buffer.front(), value);
        EXPECT_EQ(buffer.back(), value);
    }
    {
        const int value = 2;
        buffer.push_back(value);
        EXPECT_EQ(buffer.size(), 1);
        EXPECT_EQ(buffer.at(0), value);
        EXPECT_EQ(buffer.front(), value);
        EXPECT_EQ(buffer.back(), value);
    }
}

TEST(ring_buffer, ring_buffer_size_3)
{
    constexpr size_t capacity = 3;
    ring_buffer<int, capacity> buffer;
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.capacity(), capacity);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    EXPECT_EQ(buffer.size(), 3);
    EXPECT_EQ(buffer.at(0), 1);
    EXPECT_EQ(buffer.at(1), 2);
    EXPECT_EQ(buffer.at(2), 3);
    EXPECT_EQ(buffer.front(), 1);
    EXPECT_EQ(buffer.back(), 3);
}

TEST(ring_buffer, iterator)
{
    constexpr int capacity = 4;
    ring_buffer<char, capacity> buffer;
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.capacity(), capacity);
    EXPECT_TRUE(buffer.empty());
    for (char i = 0; i < capacity; ++i)
    {
        buffer.push_back(i + '0');
    }
    for (char& v : buffer)
    {
        ++v;
    }
    for (int i = 0; i < capacity; ++i)
    {
        EXPECT_EQ(buffer[i], i + '1');
    }

    auto expectBufferEq = [&](const char* expect, const char* reversed, const char* data) {
        std::string str;
        for (const auto& c : buffer)
        {
            str.push_back(c);
        }
        EXPECT_EQ(str, expect);
        str.clear();
        for (auto it = buffer.crbegin(); it != buffer.crend(); ++it)
        {
            str.push_back(*(it));
        }
        EXPECT_EQ(str, reversed);
        str.clear();
        for (auto it = buffer.rbegin(); it != buffer.rend(); ++it)
        {
            str.push_back(*(it));
        }
        EXPECT_EQ(str, reversed);
        str.clear();
        EXPECT_EQ(std::string(buffer.data(), buffer.data() + buffer.capacity()), data);
    };

    // | 1, 2, 3, 4
    expectBufferEq("1234", "4321", "1234");

    buffer.pop_front();
    EXPECT_EQ(buffer.size(), capacity - 1);
    EXPECT_EQ(buffer.front(), '2');
    EXPECT_EQ(buffer.back(), '4');
    // x | 2, 3, 4
    expectBufferEq("234", "432", "1234");

    // 7 | 2, 3, 4
    buffer.push_back('7');
    expectBufferEq("2347", "7432", "7234");

    // 7, 8 | 3, 4
    buffer.push_back('8');
    expectBufferEq("3478", "8743", "7834");

    EXPECT_EQ(buffer.at(0), '3');
    EXPECT_EQ(buffer.at(1), '4');
    EXPECT_EQ(buffer.at(2), '7');
    EXPECT_EQ(buffer.at(3), '8');

    // 7, 8 | x, 4
    buffer.pop_front();
    expectBufferEq("478", "874", "7834");
}
