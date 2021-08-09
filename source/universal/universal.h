#pragma once

template<typename T, int N>
inline constexpr int array_length( T ( &buffer )[N] )
{
    return N;
}

inline constexpr unsigned int log_two( unsigned int x )
{
    return x == 1 ? 0 : 1 + log_two( x >> 1 );
}

inline constexpr bool is_power_of_two( unsigned int x )
{
    return ( x & ( x - 1 ) ) == 0;
}
