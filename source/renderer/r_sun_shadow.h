#pragma once

#include "r_defines.h"
#include "scene/Scene.h"

void LightSpaceMatrix( const Camera& camera, vec3 lightDir, mat4 lightPVs[NUM_CASCADES] );