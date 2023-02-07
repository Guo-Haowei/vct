#include "D3dCommon.hpp"

#include <cstdio>

void DXCallImpl( HRESULT result, const char *file, int ln, const char *expr )
{
    if ( result != S_OK ) {
        char buffer[1024]{ 0 };
        snprintf( buffer, sizeof( buffer ),
                  "*** D3D Error ***\n"
                  "\t%s\n"
                  "\t%s(%d,1)",
                  expr, file, ln );
        base::Print( FOREGROUND_RED, buffer );

        __debugbreak();
    }
}