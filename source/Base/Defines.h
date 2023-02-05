#pragma once
#include <cstdint>

#define IN_USE      &&
#define NOT_IN_USE  &&!
#define USE_IF( x ) &&( ( x ) ? 1 : 0 )&&
#define USING( x )  ( 1 x 1 )

#if defined( _DEBUG ) || defined( DDEBUG )
#define DEBUG_BUILD   IN_USE
#define RELEASE_BUILD NOT_IN_USE
#else
#define DEBUG_BUILD   NOT_IN_USE
#define RELEASE_BUILD IN_USE
#endif

#define ENABLE_ASSERT IN_USE
#define ENABLE_LOG    IN_USE
#define DEBUG_GFX     IN_USE

#define ENABLE_IMGUI_RENDERING IN_USE

#define ALIGN( x, a ) ( ( ( x ) + ( (a)-1 ) ) & ~( (a)-1 ) )

inline constexpr size_t KB = 1024;
inline constexpr size_t MB = 1024 * KB;
inline constexpr size_t GB = 1024 * MB;

template<typename T, int N>
inline constexpr int array_length( T ( & )[N] )
{
    return N;
}

template<typename T>
void unused( T& ){};

inline constexpr uint32_t log_two( uint32_t x )
{
    return x == 1 ? 0 : 1 + log_two( x >> 1 );
}

inline constexpr bool is_power_of_two( uint32_t x )
{
    return ( x & ( x - 1 ) ) == 0;
}