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
    explicit this_type(pointer ptr) : mPtr(ptr) {}

    this_type operator++(int)
    {
        this_type tmp = *this;
        ++mPtr;
        return tmp;
    }

    this_type& operator++()
    {
        ++mPtr;
        return *this;
    }

    this_type operator--(int)
    {
        this_type tmp = *this;
        --mPtr;
        return tmp;
    }

    this_type& operator--()
    {
        --mPtr;
        return *this;
    }

    reference operator*() const
    {
        return *mPtr;
    }

    pointer operator->() const
    {
        return mPtr;
    }

    bool operator==(const this_type& rhs) const
    {
        return mPtr == rhs.mPtr;
    }

    bool operator!=(const this_type& rhs) const
    {
        return mPtr != rhs.mPtr;
    }

private:
    pointer mPtr;
};

template<typename base_iterator_type>
class reverse_iterator_type
{
    using this_type = reverse_iterator_type<base_iterator_type>;
    using element_type = typename base_iterator_type::element_type;
    using reference = element_type&;
    using pointer = element_type*;

public:
    explicit this_type(const base_iterator_type& iterator) : mInternal(iterator) {}

    this_type operator++(int)
    {
        this_type tmp = *this;
        --mInternal;
        return tmp;
    }

    this_type& operator++()
    {
        --mInternal;
        return *this;
    }

    reference operator*() const
    {
        return mInternal.operator*();
    }

    pointer operator->() const
    {
        return mInternal.operator->();
    }

    bool operator==(const this_type& rhs) const
    {
        return mInternal == rhs.mInternal;
    }

    bool operator!=(const this_type& rhs) const
    {
        return mInternal != rhs.mInternal;
    }

private:
    base_iterator_type mInternal;
};
