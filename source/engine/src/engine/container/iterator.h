#pragma once

#define DEFAULT_ITERATOR_FUNCTIONS()                                                    \
    const_iterator cbegin() const { return begin(); }                                   \
    const_iterator cend() const { return end(); }                                       \
    reverse_iterator rbegin() { return reverse_iterator(--end()); }                     \
    reverse_iterator rend() { return reverse_iterator(--begin()); }                     \
    reverse_const_iterator rbegin() const { return reverse_const_iterator(--end()); }   \
    reverse_const_iterator rend() const { return reverse_const_iterator(--begin()); }   \
    reverse_const_iterator crbegin() const { return reverse_const_iterator(--cend()); } \
    reverse_const_iterator crend() const { return reverse_const_iterator(--cbegin()); }

template<typename T>
class linear_iterator
{
public:
    using this_type = linear_iterator<T>;
    using element_type = T;
    using reference = element_type&;
    using pointer = element_type*;

public:
    explicit this_type(pointer ptr) : m_ptr(ptr) {}

    this_type operator++(int)
    {
        this_type tmp = *this;
        ++m_ptr;
        return tmp;
    }

    this_type& operator++()
    {
        ++m_ptr;
        return *this;
    }

    this_type operator--(int)
    {
        this_type tmp = *this;
        --m_ptr;
        return tmp;
    }

    this_type& operator--()
    {
        --m_ptr;
        return *this;
    }

    reference operator*() const
    {
        return *m_ptr;
    }

    pointer operator->() const
    {
        return m_ptr;
    }

    bool operator==(const this_type& rhs) const
    {
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const this_type& rhs) const
    {
        return m_ptr != rhs.m_ptr;
    }

private:
    pointer m_ptr;
};

template<typename base_iterator_type>
class reverse_iterator_type
{
    using this_type = reverse_iterator_type<base_iterator_type>;
    using element_type = typename base_iterator_type::element_type;
    using reference = element_type&;
    using pointer = element_type*;

public:
    explicit this_type(const base_iterator_type& iterator) : m_internal(iterator) {}

    this_type operator++(int)
    {
        this_type tmp = *this;
        --m_internal;
        return tmp;
    }

    this_type& operator++()
    {
        --m_internal;
        return *this;
    }

    reference operator*() const
    {
        return m_internal.operator*();
    }

    pointer operator->() const
    {
        return m_internal.operator->();
    }

    bool operator==(const this_type& rhs) const
    {
        return m_internal == rhs.m_internal;
    }

    bool operator!=(const this_type& rhs) const
    {
        return m_internal != rhs.m_internal;
    }

private:
    base_iterator_type m_internal;
};
