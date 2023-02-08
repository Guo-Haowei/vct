#include "Assert.h"

#ifdef UNICODE
#undef UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "StringUtils.hpp"

#include "Logger.h"

namespace base {

void Assert( const char* file, int ln, const char* expr )
{
    char buffer[1024];
    Sprintf( buffer,
             "*** assertion failed ***\n"
             "\t%s\n"
             "\t%s(%d,1)",
             expr, file, ln );
    Print( FOREGROUND_RED, buffer );
    __debugbreak();
}

}  // namespace base
