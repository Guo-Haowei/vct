#pragma once
#include "fixed_stack.h"

template<typename T, size_t N>
class ring_buffer;

template<typename T, size_t N>
class ring_buffer_iterator
{
public:
    using this_type = ring_buffer_iterator<T, N>;
    using element_type = T;
    using reference = element_type&;
    using pointer = element_type*;

public:
    ring_buffer_iterator(size_t index, pointer ptr)
    {
        m_index = index + N;  // add N to make sure when --, index doesn't go under 0
        m_ptr = ptr;
    }

    this_type operator++(int)
    {
        this_type tmp = *this;
        ++m_index;
        return tmp;
    }

    this_type& operator++()
    {
        ++m_index;
        return *this;
    }

    this_type operator--(int)
    {
        this_type tmp = *this;
        --m_index;
        return tmp;
    }

    this_type& operator--()
    {
        --m_index;
        return *this;
    }

    reference operator*() const
    {
        return m_ptr[m_index % N];
    }

    pointer operator->() const
    {
        return &m_ptr[m_index % N];
    }

    bool operator==(const this_type& rhs) const
    {
        assert(m_ptr == rhs.m_ptr);
        return m_index == rhs.m_index;
    }

    bool operator!=(const this_type& rhs) const
    {
        assert(m_ptr == rhs.m_ptr);
        return m_index != rhs.m_index;
    }

private:
    size_t m_index;
    pointer m_ptr;
};

template<typename T, size_t N>
class ring_buffer : public fixed_stack<T, N>
{
public:
    using this_type = ring_buffer<T, N>;
    using base_type = fixed_stack<T, N>;

    using iterator = ring_buffer_iterator<T, N>;
    using const_iterator = ring_buffer_iterator<const T, N>;
    using reverse_iterator = reverse_iterator_type<iterator>;
    using reverse_const_iterator = reverse_iterator_type<const_iterator>;

public:
    iterator begin() { return iterator(m_head, m_data); }
    iterator end() { return iterator(m_head + m_size, m_data); }
    const_iterator begin() const { return const_iterator(m_head, m_data); }
    const_iterator end() const { return const_iterator(m_head + m_size, m_data); }

    DEFAULT_ITERATOR_FUNCTIONS();

public:
    ring_buffer()
    {
        clear();
    }

    void clear()
    {
        m_head = 0;
        m_size = 0;
    }

    void push_back(const T& value)
    {
        if (m_size < N)
        {
            ++m_size;
            m_data[index(m_size - 1)] = value;
        }
        else
        {
            m_data[index(0)] = value;
            m_head = (m_head + 1) % N;
        }
    }

    void pop_front()
    {
        assert(!empty());
        m_head = (m_head + 1) % N;
        --m_size;
    }

    T& front()
    {
        assert(!empty());
        return m_data[index(0)];
    }

    T& back()
    {
        assert(!empty());
        return m_data[index(m_size - 1)];
    }

    inline T& at(size_t idx) { return m_data[index(idx)]; }

    inline const T& at(size_t idx) const { return m_data[index(idx)]; }

    inline T& operator[](size_t idx) { return at(idx); }

    inline const T& operator[](size_t idx) const { return at(idx); }

private:
    inline size_t index(size_t idx) const
    {
        assert(idx < m_size);
        return (m_head + idx) % N;
    }

private:
    size_t m_head;
};