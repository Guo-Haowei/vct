#include "Camera.h"

namespace vct {

mat4 Camera::view() const
{
    return glm::lookAt( position, position + direction(), vec3( 0, 1, 0 ) );
}

vec3 Camera::direction() const
{
    return vec3(
        glm::cos( glm::radians( yaw ) ) * std::cos( glm::radians( pitch ) ),
        glm::sin( glm::radians( pitch ) ),
        glm::sin( glm::radians( yaw ) ) * std::cos( glm::radians( pitch ) ) );
}

mat4 Camera::perspective() const
{
    return glm::perspectiveRH_NO( fovy, aspect, zNear, zFar );
}

}  // namespace vct
