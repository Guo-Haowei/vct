#include "core_assert.h"

#include <cstdarg>
#include <cstdio>

#include "print.h"

#define endpoint stdout

namespace detail {

static constexpr size_t kMaxBufSize = 1024;

void Assert( const char* file, int ln, const char* expr )
{
    Com_PrintFatal( "*** assertion ***\n\t%s\n\t%s(%d,1)", expr, file, ln );
}

void Assertfmt( const char* file, int ln, const char* expr, const char* fmt, ... )
{
    char buf[kMaxBufSize];
    va_list args;
    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ) - 1, fmt, args );
    va_end( args );
    Com_PrintFatal( "*** assertion ***\n\t%s failed (%s).\n\t%s(%d,1)", buf, expr, file, ln );
}

void Panic( const char* file, int ln, const char* fmt, ... )
{
    char buf[kMaxBufSize];
    va_list args;
    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ) - 1, fmt, args );
    va_end( args );
    Com_PrintFatal( "*** panic ***\n\t%s\n\t%s(%d,1)", buf, file, ln );
}

}  // namespace detail
