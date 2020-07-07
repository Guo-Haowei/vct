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
    // Vector3 front   = -Vector3::UnitZ;
    // Vector3 right   = Vector3::UnitX;
    // Vector3 up      = Vector3::UnitY;
    Vector3 position  = Vector3::Zero;
};

} // namespace vct
