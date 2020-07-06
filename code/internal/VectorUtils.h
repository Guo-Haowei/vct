#pragma once
#include <vector>
#include <iostream>

namespace internal {

template <class T> size_t vectorSize(const std::vector<T>& buffer)
{
    return buffer.size() * sizeof(T);
}

} // namespace internal

