#pragma once
#include <memory>
#include <type_traits>
#include <string>
#include <vector>

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
constexpr std::underlying_type_t<T> underlying(T value)
{
    static_assert(std::is_enum_v<T>);
    return static_cast<std::underlying_type_t<T>>(value);
}

#define DISABLE_COPY(CLASS)       \
    CLASS(const CLASS&) = delete; \
    CLASS& operator=(const CLASS&) = delete

using String = std::string;
using StringVector = std::vector<String>;

template<typename FUNC>
class ScopeDrop
{
public:
    ScopeDrop(FUNC func) : mFunc(func) {}
    ~ScopeDrop() { mFunc(); }

private:
    FUNC mFunc;
};

template<typename FUNC>
ScopeDrop<FUNC> MakeScopeDrop(FUNC func)
{
    return ScopeDrop<FUNC>(func);
}

#define on_scope_exit(FUNC) auto __on_scope_exit_call = ::MakeScopeDrop(FUNC)
