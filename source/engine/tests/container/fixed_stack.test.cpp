#include <engine/container/fixed_stack.h>

TEST(fixed_stack, initializer)
{
    const fixed_stack<char, 16> a{ 'a', 'b', 'c' };
    EXPECT_EQ(a[0], 'a');
    EXPECT_EQ(a.at(1), 'b');
}

TEST(fixed_stack, push_back)
{
    fixed_stack<char, 16> name;
    EXPECT_TRUE(name.empty());
    for (char i = 0; i < 3; ++i)
    {
        name.push_back(i + 'a');
    }

    EXPECT_EQ(*name.data(), 'a');
    EXPECT_EQ(name[1], 'b');
    EXPECT_EQ(name.at(2), 'c');
}

TEST(fixed_stack, iterator)
{
    fixed_stack<char, 16> buffer;
    fixed_stack<char, 16> buffer2{ 'a', 'b', 'c' };

    for (auto it = buffer2.rbegin(); it != buffer2.rend(); ++it)
    {
        buffer.push_back(*(it));
    }
    // buffer -> "cba"
    for (auto it = buffer2.crbegin(); it != buffer2.crend(); ++it)
    {
        buffer.push_back(*(it) + '0' - 'a');
    }
    // buffer -> "cba210"
    for (auto it = buffer2.begin(); it != buffer2.end(); ++it)
    {
        (*it) += 1;
    }
    for (auto it = buffer2.cbegin(); it != buffer2.cend(); ++it)
    {
        buffer.push_back(*(it));
    }

    // buffer -> "cba210bcd"
    EXPECT_EQ(buffer[0], 'c');
    EXPECT_EQ(buffer[1], 'b');
    EXPECT_EQ(buffer[2], 'a');
    EXPECT_EQ(buffer[3], '2');
    EXPECT_EQ(buffer[4], '1');
    EXPECT_EQ(buffer[5], '0');
    EXPECT_EQ(buffer[6], 'b');
    EXPECT_EQ(buffer[7], 'c');
    EXPECT_EQ(buffer[8], 'd');
}
