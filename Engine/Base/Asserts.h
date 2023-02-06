#pragma once
#include "defines.h"

#if USING( ENABLE_ASSERT )
#define ASSERT( expr )            (void)( ( !!( expr ) ) || ( base::Assert( __FILE__, __LINE__, #expr ), 0 ) )
#define ASSERTRANGE( expr, a, b ) ASSERT( ( ( expr >= a ) && ( expr <= b ) ) )
#define UNREACHABLE()             base::Assert( __FILE__, __LINE__, "should not reach here" )
#else
#define ASSERT( ... )      (void)0
#define ASSERTRANGE( ... ) (void)0
#define UNREACHABLE( ... ) (void)0
#endif

namespace base {

void Assert( const char* file, int ln, const char* expr );

}
