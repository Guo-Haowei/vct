#include "camera.h"

vec3 Camera::direction() const
{
    return vec3(
        glm::cos( glm::radians( yaw ) ) * glm::cos( glm::radians( pitch ) ),
        glm::sin( glm::radians( pitch ) ),
        glm::sin( glm::radians( yaw ) ) * glm::cos( glm::radians( pitch ) ) );
}

void Camera::UpdatePV()
{
    mView = glm::lookAt( position, position + direction(), vec3( 0, 1, 0 ) );
#if 0
    mProj = glm::perspectiveRH_NO( fovy, mAspect, zNear, zFar );
#else
    mProj = mat4( { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0.5, 0 }, { 0, 0, 0, 1 } ) *
            mat4( { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 1, 1 } ) *
            glm::perspectiveRH_NO( fovy, mAspect, zNear, zFar );
#endif
}