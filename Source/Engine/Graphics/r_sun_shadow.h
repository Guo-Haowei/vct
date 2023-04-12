#pragma once

#include "Core/scene.h"
#include "r_defines.h"

void R_LightSpaceMatrix(const Camera& camera, const vec3& lightDir, mat4 lightPVs[NUM_CASCADES]);

void R_ShadowPass();