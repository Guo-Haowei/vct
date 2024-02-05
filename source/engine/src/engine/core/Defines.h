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
