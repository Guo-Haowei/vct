#pragma once

#include "common/scene.h"
#include "r_defines.h"

void R_LightSpaceMatrix( const Camera& camera, vec3 lightDir, mat4 lightPVs[NUM_CASCADES] );

void R_ShadowPass();