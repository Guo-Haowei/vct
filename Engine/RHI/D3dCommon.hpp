#pragma once
#include <type_traits>
#include <wrl/client.h>
#include "Base/Logger.h"

using Microsoft::WRL::ComPtr;

template<typename T>
inline void SafeRelease( T *&p )
{
    static_assert( !std::is_pointer<T>() );
    if ( p ) {
        p->Release();
        p = nullptr;
    }
}

#if USING( DEBUG_GFX )
#define DX_CALL( x ) DXCallImpl( ( x ), __FILE__, __LINE__, #x )
#else
#define DX_CALL( x ) ( x )
#endif

HRESULT DXCallImpl( HRESULT result, const char *file, int ln, const char *expr );