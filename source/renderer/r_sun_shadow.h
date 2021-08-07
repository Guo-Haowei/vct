#pragma once

#include "r_defines.h"
#include "scene/Scene.h"

void R_LightSpaceMatrix( const Camera& camera, vec3 lightDir, mat4 lightPVs[NUM_CASCADES] );

void R_ShadowPass();