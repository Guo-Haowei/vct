#include "Camera.h"

vec3 Camera::direction() const
{
    return vec3(
        glm::cos( glm::radians( yaw ) ) * glm::cos( glm::radians( pitch ) ),
        glm::sin( glm::radians( pitch ) ),
        glm::sin( glm::radians( yaw ) ) * glm::cos( glm::radians( pitch ) ) );
}

void Camera::UpdatePV()
{
    mView     = glm::lookAt( position, position + direction(), vec3( 0, 1, 0 ) );
    mProj     = glm::perspectiveRH_NO( fovy, mAspect, zNear, zFar );
    mProjView = mProj * mView;
}