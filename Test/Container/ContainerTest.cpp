#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "Container/fixed_stack.h"
#include "Container/fixed_string.h"
#include "Container/ring_buffer.h"

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

    fixed_string<16> buffer;

    for (const auto& it : name)
    {
        buffer.push_back(it);
    }

    CHECK(buffer == "abc");
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

    CHECK(buffer == "cba210bcd");
    CHECK(memcmp(name.data(), "bcd", name.size()) == 0);

    CHECK(name.size() == 3);
    CHECK(name.capacity() == 16);
}

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

TEST_CASE("ring_buffer")
{
    SUBCASE("ring_buffer<1>")
    {
        constexpr size_t capacity = 1;
        ring_buffer<int, capacity> buffer;
        CHECK(buffer.size() == 0);
        CHECK(buffer.capacity() == capacity);
        CHECK(buffer.empty());
        {
            const int value = 1;
            buffer.push_back(value);
            CHECK(buffer.size() == 1);
            CHECK(buffer.at(0) == value);
            CHECK(buffer.front() == value);
            CHECK(buffer.back() == value);
        }
        {
            const int value = 2;
            buffer.push_back(value);
            CHECK(buffer.size() == 1);
            CHECK(buffer.at(0) == value);
            CHECK(buffer.front() == value);
            CHECK(buffer.back() == value);
        }
    }

    SUBCASE("ring_buffer<4>")
    {
        constexpr size_t capacity = 3;
        ring_buffer<int, capacity> buffer;
        CHECK(buffer.size() == 0);
        CHECK(buffer.capacity() == capacity);
        CHECK(buffer.empty());
        {
            const int v1 = 1;
            const int v2 = 2;
            const int v3 = 3;
            buffer.push_back(v1);
            buffer.push_back(v2);
            buffer.push_back(v3);
            CHECK(buffer.size() == 3);
            CHECK(buffer.at(0) == v1);
            CHECK(buffer.at(1) == v2);
            CHECK(buffer.at(2) == v3);
            CHECK(buffer.front() == v1);
            CHECK(buffer.back() == v3);
        }
    }

    SUBCASE("ring_buffer_iterator")
    {
        constexpr int capacity = 4;
        ring_buffer<char, capacity> buffer;
        CHECK(buffer.size() == 0);
        CHECK(buffer.capacity() == capacity);
        CHECK(buffer.empty());
        for (int i = 0; i < capacity; ++i)
        {
            buffer.push_back(i + '0');
        }
        for (char& v : buffer)
        {
            ++v;
        }
        for (int i = 0; i < capacity; ++i)
        {
            CHECK(buffer[i] == i + '1');
        }

        auto checkBuffer = [&](const char* expect, const char* reversed, const char* data) {
            std::string str;
            for (const auto& c : buffer)
            {
                str.push_back(c);
            }
            CHECK(str == expect);
            str.clear();
            for (auto it = buffer.crbegin(); it != buffer.crend(); ++it)
            {
                str.push_back(*(it));
            }
            CHECK(str == reversed);
            str.clear();
            for (auto it = buffer.rbegin(); it != buffer.rend(); ++it)
            {
                str.push_back(*(it));
            }
            CHECK(str == reversed);
            str.clear();
            CHECK(std::string(buffer.data(), buffer.data() + buffer.capacity()) == data);
        };

        // | 1, 2, 3, 4
        checkBuffer("1234", "4321", "1234");

        buffer.pop_front();
        CHECK(buffer.size() == capacity - 1);
        CHECK(buffer.front() == '2');
        CHECK(buffer.back() == '4');
        // x | 2, 3, 4
        checkBuffer("234", "432", "1234");

        // 7 | 2, 3, 4
        buffer.push_back('7');
        checkBuffer("2347", "7432", "7234");

        // 7, 8 | 3, 4
        buffer.push_back('8');
        checkBuffer("3478", "8743", "7834");

        CHECK(buffer.at(0) == '3');
        CHECK(buffer.at(1) == '4');
        CHECK(buffer.at(2) == '7');
        CHECK(buffer.at(3) == '8');

        // 7, 8 | x, 4
        buffer.pop_front();
        checkBuffer("478", "874", "7834");
    }
}
