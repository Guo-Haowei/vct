#pragma once

#ifdef _STR
#undef _STR
#endif  // !_STR
#define _STR(x) #x

#define ON_SCOPE_EXIT(FUNC) auto __on_scope_exit_call = ::vct::MakeScopeDrop(FUNC)

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

template<typename FUNC>
class ScopeDrop {
public:
    ScopeDrop(FUNC func) : m_func(func) {}
    ~ScopeDrop() { m_func(); }

private:
    FUNC m_func;
};

template<typename FUNC>
ScopeDrop<FUNC> MakeScopeDrop(FUNC func) {
    return ScopeDrop<FUNC>(func);
}

}  // namespace vct
