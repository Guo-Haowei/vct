
#include <engine/container/fixed_stack.h>

TEST_CASE("fixed_stack")
{
    using String = fixed_stack<char, 16>;
    SUBCASE("initializer")
    {
        const String a = { 'a', 'b', 'c' };
        CHECK(a[0] == 'a');
        CHECK(a.at(1) == 'b');
    }

    String name;
    CHECK(name.empty());
    for (int i = 0; i < 3; ++i)
    {
        name.push_back(i + 'a');
    }

    CHECK(*name.data() == 'a');
    CHECK(name[1] == 'b');
    CHECK(name.at(2) == 'c');

    fixed_stack<char, 16> buffer;

    for (const auto& it : name)
    {
        buffer.push_back(it);
    }

    CHECK(buffer[0] == 'a');
    CHECK(buffer[1] == 'b');
    CHECK(buffer[2] == 'c');
    CHECK(buffer.size() == 3);
    buffer.clear();

    for (auto it = name.rbegin(); it != name.rend(); ++it)
    {
        buffer.push_back(*(it));
    }
    // buffer -> "cba"
    for (auto it = name.crbegin(); it != name.crend(); ++it)
    {
        buffer.push_back(*(it) + '0' - 'a');
    }
    // buffer -> "cba210"
    for (auto it = name.begin(); it != name.end(); ++it)
    {
        (*it) += 1;
    }
    for (auto it = name.cbegin(); it != name.cend(); ++it)
    {
        buffer.push_back(*(it));
    }
    // buffer -> "cba210bcd"

    CHECK(buffer[0] == 'c');
    CHECK(buffer[1] == 'b');
    CHECK(buffer[2] == 'a');
    CHECK(buffer[3] == '2');
    CHECK(buffer[4] == '1');
    CHECK(buffer[5] == '0');
    CHECK(buffer[6] == 'b');
    CHECK(buffer[7] == 'c');
    CHECK(buffer[8] == 'd');

    CHECK(memcmp(name.data(), "bcd", name.size()) == 0);

    CHECK(name.size() == 3);
    CHECK(name.capacity() == 16);
}
