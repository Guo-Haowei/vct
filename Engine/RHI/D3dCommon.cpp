#include "D3dCommon.hpp"

#include "Base/StringUtils.hpp"

HRESULT DXCallImpl( HRESULT hr, const char *file, int ln, const char *expr )
{
    if ( hr != S_OK ) {
        char buffer[1024];
        Sprintf( buffer,
                 "*** D3D Error ***\n"
                 "\t%s\n"
                 "\t%s(%d,1)",
                 expr, file, ln );
        base::Print( FOREGROUND_RED, buffer );

        if ( IsDebuggerPresent() ) {
            __debugbreak();
        }
    }

    return hr;
}