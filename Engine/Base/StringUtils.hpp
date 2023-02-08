#pragma once
#include <cstdarg>
#include <cstdio>
#include <string>

template<int size>
int Sprintf( char ( &dest )[size], const char *const fmt, ... )
{
    va_list argptr;
    va_start( argptr, fmt );
    int len = vsnprintf( dest, size, fmt, argptr );
    va_end( argptr );
    dest[size - 1] = 0;
    return len;
}
