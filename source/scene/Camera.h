#pragma once
#include "base/GeoMath.h"

namespace vct {

struct Camera
{
    Matrix4 view() const;
    Matrix4 perspective() const;
    Vector3 direction() const;

    float fovy;
    float aspect;
    float zNear;
    float zFar;
    float yaw = -90.0f;
    float pitch = 0.0f;
    Vector3 position  = Vector3::Zero;
    Frustum frustum;
};

} // namespace vct
