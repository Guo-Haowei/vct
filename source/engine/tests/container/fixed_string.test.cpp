#include <engine/container/fixed_string.h>

TEST(fixed_string, operators)
{
    fixed_string<32> name;
    EXPECT_EQ(name.size(), 1);
    EXPECT_EQ(name.length(), 0);
    EXPECT_TRUE(name.empty());

    name = "abc";
    EXPECT_EQ(name.size(), 4);
    EXPECT_EQ(name.length(), 3);
    EXPECT_TRUE(!name.empty());
    EXPECT_EQ(name, std::string("abc"));

    name = std::string("wxyz");
    EXPECT_EQ(name, "wxyz");
    EXPECT_EQ(name, fixed_string<5>("wxyz"));

    EXPECT_TRUE(name.iequal("wXyZ"));
    EXPECT_TRUE(!name.iequal("wXyZa"));

    std::string buffer;

    for (auto& c : name)
    {
        c = c - 'w' + 'a';
    }

    for (const auto& c : name)
    {
        buffer.push_back(c);
    }

    for (auto it = name.crbegin(); it != name.crend(); ++it)
    {
        buffer.push_back((*it) - 'a' + '0');
    }

    EXPECT_EQ(buffer, "abcd3210");
}
