#include <engine/utils/ring_buffer.h>

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
