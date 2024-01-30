#pragma once

namespace vct
{

template<typename T>
class LinearIterator
{
    using this_type = LinearIterator<T>;

public:
    explicit LinearIterator(T* ptr) : m_ptr(ptr) {}

    this_type operator++(int)
    {
        LinearIterator<T> tmp = *this;
        ++m_ptr;
        return tmp;
    }

    this_type operator--(int)
    {
        this_type tmp = *this;
        --m_ptr;
        return tmp;
    }

    this_type& operator++()
    {
        ++m_ptr;
        return *this;
    }

    this_type& operator--()
    {
        --m_ptr;
        return *this;
    }

    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

    bool operator==(const this_type& rhs) const { return m_ptr == rhs.m_ptr; }
    bool operator!=(const this_type& rhs) const { return m_ptr != rhs.m_ptr; }

private:
    T* m_ptr = nullptr;
};

}  // namespace vct
