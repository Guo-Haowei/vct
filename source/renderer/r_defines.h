#pragma once
#include "universal/universal.h"

inline constexpr int NUM_CASCADES = 3;

inline constexpr unsigned int SHADOW_MAP_RESOLUTION = 1024 * 4;

static_assert( is_power_of_two( SHADOW_MAP_RESOLUTION ) );
