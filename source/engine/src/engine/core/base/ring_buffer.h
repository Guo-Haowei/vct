#pragma once
#include <cassert>
#include <initializer_list>

#include "reverse_iterator.h"

namespace vct {

template<typename T, size_t N>
class RingBufferIterator {
    using self_type = RingBufferIterator<T, N>;

public:
    RingBufferIterator(size_t index, T* ptr) : m_index(index), m_ptr(ptr) {}

    self_type operator++(int) { return self_type(m_index++, m_ptr); }

    self_type& operator++() {
        ++m_index;
        return *this;
    }

    self_type operator--(int) { return self_type(m_index--, m_ptr); }

    self_type& operator--() {
        --m_index;
        return *this;
    }

    T& operator*() const { return m_ptr[(m_index + N) % N]; }

    T* operator->() const { return &m_ptr[(m_index + N) % N]; }

    bool operator==(const self_type& rhs) const {
        assert(m_ptr == rhs.m_ptr);
        return m_index == rhs.m_index;
    }

    bool operator!=(const self_type& rhs) const {
        assert(m_ptr == rhs.m_ptr);
        return m_index != rhs.m_index;
    }

private:
    size_t m_index;
    T* m_ptr;
};

template<typename T, size_t N>
class RingBuffer {
    using iter = RingBufferIterator<T, N>;
    using const_iter = RingBufferIterator<const T, N>;
    using reverse_iter = ReverseIterator<T, iter>;
    using reverse_const_iter = ReverseIterator<const T, const_iter>;

public:
    iter begin() { return iter(m_head, m_data); }
    iter end() { return iter(m_head + m_size, m_data); }
    const_iter begin() const { return const_iter(m_head, m_data); }
    const_iter end() const { return const_iter(m_head + m_size, m_data); }
    const_iter cbegin() const { return begin(); }
    const_iter cend() const { return end(); }
    reverse_iter rbegin() { return reverse_iter(end()); }
    reverse_iter rend() { return reverse_iter(begin()); }
    reverse_const_iter rbegin() const { return reverse_const_iter(end()); }
    reverse_const_iter rend() const { return reverse_const_iter(begin()); }
    reverse_const_iter crbegin() const { return reverse_const_iter(cend()); }
    reverse_const_iter crend() const { return reverse_const_iter(cbegin()); }

public:
    RingBuffer() = default;

    RingBuffer(std::initializer_list<T> list) {
        for (auto& element : list) {
            push_back(element);
        }
    }

    void clear() { m_head = m_size = 0; }

    bool empty() const { return m_size == 0; }
    size_t size() const { return m_size; }
    constexpr size_t capacity() const { return N; }

    auto operator[](size_t idx) -> T& { return m_data[index(idx)]; }
    auto operator[](size_t idx) const -> const T& { return m_data[index(idx)]; }

    auto front() -> T& { return (*this)[0]; }
    auto front() const -> const T& { return (*this)[0]; }
    auto back() -> T& { return (*this)[m_size - 1]; }
    auto back() const -> const T& { return (*this)[m_size - 1]; }

    void push_back(const T& value) {
        if (m_size < N) {
            ++m_size;
            m_data[index(m_size - 1)] = value;
        } else {
            m_data[index(0)] = value;
            m_head = (m_head + 1) % N;
        }
    }

    void pop_front() {
        assert(!empty());
        m_head = (m_head + 1) % N;
        --m_size;
    }

private:
    size_t index(size_t idx) const {
        // @TODO: bound check
        assert(idx < m_size);
        return (m_head + idx) % N;
    }

private:
    size_t m_head = 0;
    size_t m_size = 0;
    T m_data[N];
};
}  // namespace vct