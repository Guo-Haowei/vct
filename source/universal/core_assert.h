#pragma once
#include "platform_defines.h"

#if USING( TEST_BUILD )
#define core_assert( expr )              (void)( ( !!( expr ) ) || ( detail::Assert( __FILE__, __LINE__, #expr ), 0 ) )
#define core_assertfmt( expr, fmt, ... ) (void)( ( !!( expr ) ) || ( detail::Assertfmt( __FILE__, __LINE__, #expr, fmt, ##__VA_ARGS__ ), 0 ) )
#define core_assertrange( expr, a, b )   core_assert( ( ( expr >= a ) && ( expr <= b ) ) )
#else
#define core_assert( expr )              ( (void)0 )
#define core_assertfmt( expr, fmt, ... ) ( (void)0 )
#define core_assertrange( expr, a, b )   ( (void)0 )
#endif

#define panic( ... )  detail::Panic( __FILE__, __LINE__, __VA_ARGS__ )
#define unreachable() detail::Assert( __FILE__, __LINE__, "should not reach here" )

namespace detail {
void Assert( const char* file, int ln, const char* expr );
void Assertfmt( const char* file, int ln, const char* expr, const char* fmt, ... );
void Panic( const char* file, int ln, const char* fmt, ... );
}  // namespace detail
