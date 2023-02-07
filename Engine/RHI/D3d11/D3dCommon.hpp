#pragma once

#include <type_traits>
#include <d3d11.h>
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
#define DX_CALL( x )                               \
    do {                                           \
        HRESULT _hr = ( x );                       \
        DXCallImpl( _hr, __FILE__, __LINE__, #x ); \
    } while ( 0 )
#else
#define DX_CALL( x ) x
#endif

void DXCallImpl( HRESULT result, const char *file, int ln, const char *expr );