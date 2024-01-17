#pragma once
#include <cassert>
#include <initializer_list>
#include "iterator.h"

template<typename T>
const T& min_val(const T& a, const T& b)
{
    return a < b ? a : b;
}

template<typename T>
const T& max_val(const T& a, const T& b)
{
    return a > b ? a : b;
}

template<typename T, size_t N>
class fixed_stack
{
public:
    using this_type = fixed_stack<T, N>;

    using iterator = linear_iterator<T>;
    using const_iterator = linear_iterator<const T>;
    using reverse_iterator = reverse_iterator_type<iterator>;
    using reverse_const_iterator = reverse_iterator_type<const_iterator>;

public:
    iterator begin() { return iterator(data()); }
    iterator end() { return iterator(data() + m_size); }
    const_iterator begin() const { return const_iterator(data()); }
    const_iterator end() const { return const_iterator(data() + m_size); }

    DEFAULT_ITERATOR_FUNCTIONS();

public:
    T& operator[](size_t index) { return data()[index]; }
    const T& operator[](size_t index) const { return data()[index]; }

public:
    this_type()
    {
        clear();
    }

    this_type(std::initializer_list<T> list)
    {
        assert(list.size() <= N);
        clear();
        for (const T& ele : list)
        {
            push_back(ele);
        }
    }

    void clear()
    {
        m_size = 0;
    }

    void resize(size_t newSize)
    {
        m_size = newSize;
    }

    size_t capacity() const { return N; }

    bool empty() const { return m_size == 0; }

    size_t size() const { return m_size; }

    T* data() { return m_data; }
    const T* data() const { return m_data; }

    T& at(size_t index)
    {
        assert(index < m_size);
        return m_data[index];
    }

    const T& at(size_t index) const
    {
        assert(index < m_size);
        return m_data[index];
    }

    void push_back(const T& element)
    {
        assert(size() < N);
        m_data[m_size++] = element;
    }

    void pop_back()
    {
        assert(0 && "Not Implemented");
    }

protected:
    T m_data[N];
    size_t m_size;
};
