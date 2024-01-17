#include <engine/utils/fixed_string.h>

TEST_CASE("fixed_string")
{
    fixed_string<32> name;
    CHECK(name.size() == 1);
    CHECK(name.length() == 0);
    CHECK(name.empty());

    name = "abc";
    CHECK(name.size() == 4);
    CHECK(name.length() == 3);
    CHECK(!name.empty());
    CHECK(name == std::string("abc"));

    name = std::string("wxyz");
    CHECK(name == "wxyz");
    CHECK(name == fixed_string<5>("wxyz"));

    CHECK(name.iequal("wXyZ"));
    CHECK(!name.iequal("wXyZa"));

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

    CHECK(buffer == "abcd3210");
}

