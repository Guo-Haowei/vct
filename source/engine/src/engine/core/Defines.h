#pragma once
#define IN_USE     &&
#define NOT_IN_USE &&!
#define USE_IF(x)  &&((x) ? 1 : 0)&&
#define USING(x)   (1 x 1)

#if defined(_DEBUG) || defined(DDEBUG)
#define DEBUG_BUILD   IN_USE
#define RELEASE_BUILD NOT_IN_USE
#else
#define DEBUG_BUILD   NOT_IN_USE
#define RELEASE_BUILD IN_USE
#endif

#define ENABLE_CHECK       IN_USE
#define ENABLE_LOG         IN_USE
#define ENABLE_DEBUG_LAYER USE_IF(USING(DEBUG_BUILD))

#define ENABLE_IMGUI_RENDERING IN_USE

#define DISABLE_OPTIMIZATION __pragma(optimize("", off))
#define ENABLE_OPTIMIZATION  __pragma(optimize("", on))

#define PUSH_WARNING(CODE)   \
    __pragma(warning(push)); \
    __pragma(warning(disable \
                     : CODE))
#define POP_WARNING() __pragma(warning(pop))

#if defined(_WIN32)
#define PC_PROGRAM IN_USE
#else
#error "Unknown platform"
#endif

#if USING(PC_PROGRAM)
#ifdef UNICODE
#undef UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#endif

#define ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))

inline constexpr size_t KB = 1024;
inline constexpr size_t MB = 1024 * KB;
inline constexpr size_t GB = 1024 * MB;

inline constexpr int OPENGL_VERSION_MAJOR = 4;
inline constexpr int OPENGL_VERSION_MINOR = 6;

inline constexpr unsigned int log_two(unsigned int x) { return x == 1 ? 0 : 1 + log_two(x >> 1); }

inline constexpr bool is_power_of_two(unsigned int x) { return (x & (x - 1)) == 0; }

template<typename T>
void unused(T&){};
