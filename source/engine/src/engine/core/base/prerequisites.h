#pragma once
#include <cassert>

namespace vct {

#ifdef _DEBUG
inline constexpr bool kIsDebug = true;
#else
inline constexpr bool kIsDebug = false;
#endif

inline void assert_out_of_range() { assert(false && "index out of range"); }

constexpr std::size_t check_out_of_range(size_t i, size_t range) { return i < range ? i : (assert_out_of_range(), i); }

constexpr std::size_t check_out_of_range_if_debug(size_t i, size_t range) {
    return kIsDebug ? check_out_of_range(i, range) : i;
}

}  // namespace vct
