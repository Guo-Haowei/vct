#pragma once
#include <assert.h>

/// TODO: remove assert in relase build
#ifdef _DEBUG
#   define ASSERT( cond ) assert(cond);
#elif defined(_FINAL)
#   define ASSERT( cond )
#else
#   define ASSERT( cond ) assert(cond);
#endif
