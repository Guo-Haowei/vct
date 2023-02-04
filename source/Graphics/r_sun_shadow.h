#pragma once

#include "Core/scene.h"
#include "r_defines.h"

mat4 R_HackLightSpaceMatrix( const vec3& lightDir );
void R_ShadowPass();