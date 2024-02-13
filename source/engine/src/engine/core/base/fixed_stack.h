#pragma once
#include <initializer_list>

#include "linear_iterator.h"
#include "prerequisites.h"
#include "reverse_iterator.h"

namespace vct {

template<typename T, size_t N>
class FixedStack {
    using iter = LinearIterator<T>;
    using const_iter = LinearIterator<const T>;
    using reverse_iter = ReverseIterator<T, iter>;
    using reverse_const_iterator = ReverseIterator<const T, const_iter>;

public:
    iter begin() { return iter(m_data); }
    iter end() { return iter(m_data + m_size); }
    const_iter begin() const { return const_iter(m_data); }
    const_iter end() const { return const_iter(m_data + m_size); }
    const_iter cbegin() const { return begin(); }
    const_iter cend() const { return end(); }
    reverse_iter rbegin() { return reverse_iter(end()); }
    reverse_iter rend() { return reverse_iter(begin()); }
    reverse_const_iterator rbegin() const { return reverse_const_iterator(end()); }
    reverse_const_iterator rend() const { return reverse_const_iterator(begin()); }
    reverse_const_iterator crbegin() const { return reverse_const_iterator(cend()); }
    reverse_const_iterator crend() const { return reverse_const_iterator(cbegin()); }

public:
    FixedStack() = default;

    FixedStack(std::initializer_list<T> list) {
        for (auto& element : list) {
            push_back(element);
        }
    }

    void clear() { m_size = 0; }
    void resize(size_t new_size) { m_size = new_size; }

    bool empty() const { return m_size == 0; }
    size_t size() const { return m_size; }
    constexpr size_t capacity() const { return N; }

    auto data() -> T* { return m_data; }
    auto data() const -> const T* { return m_data; }

    auto operator[](size_t index) -> T& { return m_data[check_out_of_range_if_debug(index, m_size)]; }
    auto operator[](size_t index) const -> const T& { return m_data[check_out_of_range_if_debug(index, m_size)]; }

    auto front() -> T& { return (*this)[0]; }
    auto front() const -> const T& { return (*this)[0]; }
    auto back() -> T& { return (*this)[m_size - 1]; }
    auto back() const -> const T& { return (*this)[m_size - 1]; }

    void push_back(const T& v) {
        check_out_of_range_if_debug(m_size, N);
        m_data[m_size++] = v;
    }

    void pop_back() {
        check_out_of_range_if_debug(0, m_size);
        --m_size;
    }

    // @TODO: emplace back
    void emplace_back(const T& v) { push_back(v); }

protected:
    size_t m_size = 0;
    T m_data[N];
};

}  // namespace vct
