#pragma once
#include <string>

#include "fixed_stack.h"

template<size_t N>
class fixed_string : public fixed_stack<char, N>
{
public:
    using base_class = fixed_stack<char, N>;
    using this_type = fixed_string<N>;

    using iterator = typename base_class::iterator;
    using const_iterator = typename base_class::const_iterator;
    using reverse_iterator = typename base_class::reverse_iterator;
    using reverse_const_iterator = typename base_class::reverse_const_iterator;

public:
    iterator begin() { return iterator(data()); }
    iterator end() { return iterator(data() + length()); }
    const_iterator begin() const { return const_iterator(data()); }
    const_iterator end() const { return const_iterator(data() + length()); }

    DEFAULT_ITERATOR_FUNCTIONS();

public:
    this_type()
    {
        clear();
    }

    this_type(const char* c)
    {
        set_from_buffer(c);
    }

    this_type(const char* begin, const char* end)
    {
        set_from_begin_and_end(begin, end);
    }

    this_type(size_t len, const char* c)
    {
        set_from_length_and_buffer(len, c);
    }

    this_type(const std::string& str)
    {
        set_from_length_and_buffer(str.length(), str.data());
    }

    void clear()
    {
        mData[0] = 0;
        mSize = 1;
    }

    const char* c_str() const { return mData; }

    bool empty() const { return length() == 0; }

    size_t length() const { return mSize - 1; };

    bool equal(size_t n, const char* str) const
    {
        return length() == n && strncmp(mData, str, n) == 0;
    }

    bool equal(const char* str) const
    {
        return equal(strlen(str), str);
    }

    bool iequal(size_t n, const char* str) const
    {
        return length() == n && _strnicmp(mData, str, n) == 0;
    }

    bool iequal(const char* str) const
    {
        return iequal(strlen(str), str);
    }

    bool operator==(const char* str) const
    {
        return equal(str);
    }

    bool operator==(const std::string& str) const
    {
        return equal(str.length(), str.data());
    }

    template<size_t N2>
    bool operator==(const fixed_string<N2>& rhs) const
    {
        return equal(rhs.length(), rhs.data());
    }

    this_type& operator=(const std::string& rhs)
    {
        set_from_length_and_buffer(rhs.length(), rhs.data());
        return *this;
    }

    this_type& operator=(const char* rhs)
    {
        set_from_buffer(rhs);
        return *this;
    }

    void push_back(const char& element)
    {
        assert(length() < N);
        mData[length()] = element;
        mData[length() + 1] = 0;
        ++mSize;
    }

public:
    this_type& append(const char* buffer, size_t len)
    {
        assert(length() + len < N);
        memcpy(mData + length(), buffer, len);
        mSize += len;
        mData[length()] = 0;
        return *this;
    }

    this_type& append(const char* str)
    {
        return append(str, strlen(str));
    }

protected:
    void set_from_buffer(const char* c)
    {
        set_from_length_and_buffer(strlen(c), c);
    }

    void set_from_begin_and_end(const char* begin, const char* end)
    {
        assert(begin <= end);
        set_from_length_and_buffer(begin, end);
    }

    void set_from_length_and_buffer(size_t len, const char* str)
    {
        assert(len < N);
        mSize = min_val(N - 1, len) + 1;
        memcpy(mData, str, length());
        mData[length()] = 0;
    }
};
