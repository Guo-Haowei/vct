#include "Camera.h"

namespace vct {

Matrix4 Camera::view() const
{
    return three::lookAt(position, position + direction(), Vector3::UnitY);
}

Vector3 Camera::direction() const
{
    return Vector3(
        std::cos(three::radians(yaw)) * std::cos(three::radians(pitch)),
        std::sin(three::radians(pitch)),
        std::sin(three::radians(yaw)) * std::cos(three::radians(pitch))
    );
}

Matrix4 Camera::perspective() const
{
    return three::perspectiveRH_NO(fovy, aspect, zNear, zFar);
}

} // namespace vct
