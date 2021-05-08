#pragma once
#include "universal/core_math.h"

namespace vct {

struct Camera {
    mat4 view() const;
    mat4 perspective() const;
    vec3 direction() const;

    float fovy;
    float aspect;
    float zNear;
    float zFar;
    float yaw   = -90.0f;
    float pitch = 0.0f;
    vec3 position{ 0 };
    Frustum frustum;
};

}  // namespace vct
