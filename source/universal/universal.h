#pragma once

template<typename T, int N>
inline constexpr int array_length( T ( &buffer )[N] )
{
    return N;
}
