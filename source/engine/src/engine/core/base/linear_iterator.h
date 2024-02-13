#pragma once

namespace vct {

template<typename T>
class LinearIterator {
    using self_type = LinearIterator<T>;

public:
    explicit LinearIterator(T* ptr) : m_ptr(ptr) {}

    self_type operator++(int) {
        LinearIterator<T> tmp = *this;
        ++m_ptr;
        return tmp;
    }

    self_type operator--(int) {
        self_type tmp = *this;
        --m_ptr;
        return tmp;
    }

    self_type& operator++() {
        ++m_ptr;
        return *this;
    }

    self_type& operator--() {
        --m_ptr;
        return *this;
    }

    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

    bool operator==(const self_type& rhs) const { return m_ptr == rhs.m_ptr; }
    bool operator!=(const self_type& rhs) const { return m_ptr != rhs.m_ptr; }

private:
    T* m_ptr = nullptr;
};

}  // namespace vct
