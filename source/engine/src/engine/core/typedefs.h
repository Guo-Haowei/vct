#pragma once

#ifdef _STR
#undef _STR
#endif  // !_STR
#define _STR(x) #x

namespace vct {

inline constexpr size_t KB = 1024;
inline constexpr size_t MB = 1024 * KB;
inline constexpr size_t GB = 1024 * MB;

template<typename T, int N>
inline constexpr int array_length(T (&)[N]) {
    return N;
}

template<typename T>
void unused(T &) {}
}  // namespace vct
