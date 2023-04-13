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
        mIndex = index + N;  // add N to make sure when --, index doesn't go under 0
        mPtr = ptr;
    }

    this_type operator++(int)
    {
        this_type tmp = *this;
        ++mIndex;
        return tmp;
    }

    this_type& operator++()
    {
        ++mIndex;
        return *this;
    }

    this_type operator--(int)
    {
        this_type tmp = *this;
        --mIndex;
        return tmp;
    }

    this_type& operator--()
    {
        --mIndex;
        return *this;
    }

    reference operator*() const
    {
        return mPtr[mIndex % N];
    }

    pointer operator->() const
    {
        return &mPtr[mIndex % N];
    }

    bool operator==(const this_type& rhs) const
    {
        assert(mPtr == rhs.mPtr);
        return mIndex == rhs.mIndex;
    }

    bool operator!=(const this_type& rhs) const
    {
        assert(mPtr == rhs.mPtr);
        return mIndex != rhs.mIndex;
    }

private:
    size_t mIndex;
    pointer mPtr;
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
    iterator begin() { return iterator(mHead, mData); }
    iterator end() { return iterator(mHead + mSize, mData); }
    const_iterator begin() const { return const_iterator(mHead, mData); }
    const_iterator end() const { return const_iterator(mHead + mSize, mData); }

    DEFAULT_ITERATOR_FUNCTIONS();

public:
    ring_buffer()
    {
        clear();
    }

    void clear()
    {
        mHead = 0;
        mSize = 0;
    }

    void push_back(const T& value)
    {
        if (mSize < N)
        {
            ++mSize;
            mData[index(mSize - 1)] = value;
        }
        else
        {
            mData[index(0)] = value;
            mHead = (mHead + 1) % N;
        }
    }

    void pop_front()
    {
        assert(!empty());
        mHead = (mHead + 1) % N;
        --mSize;
    }

    T& front()
    {
        assert(!empty());
        return mData[index(0)];
    }

    T& back()
    {
        assert(!empty());
        return mData[index(mSize - 1)];
    }

    inline T& at(size_t idx) { return mData[index(idx)]; }

    inline const T& at(size_t idx) const { return mData[index(idx)]; }

    inline T& operator[](size_t idx) { return at(idx); }

    inline const T& operator[](size_t idx) const { return at(idx); }

private:
    inline size_t index(size_t idx) const
    {
        assert(idx < mSize);
        return (mHead + idx) % N;
    }

private:
    size_t mHead;
};