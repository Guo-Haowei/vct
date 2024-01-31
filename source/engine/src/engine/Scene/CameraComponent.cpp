#include "CameraComponent.h"

#include "Archive.h"

mat4 CameraComponent::CalculateViewMatrix() const { return glm::lookAt(eye, center, glm::normalize(vec3(0, 1, 0))); }

void CameraComponent::UpdateCamera() {
    const float aspect = width / height;
    projMatrix = glm::perspective(fovy, aspect, zNear, zFar);

    if (IsDirty()) {
        viewMatrix = CalculateViewMatrix();
        SetDirty(false);
    }
}

void CameraComponent::Serialize(Archive& archive) {
    if (archive.IsWriteMode()) {
        archive << flags;
        archive << zNear;
        archive << zFar;
        archive << fovy;
        archive << width;
        archive << height;

        archive << center;
        archive << eye;
    } else {
        archive >> flags;
        archive >> zNear;
        archive >> zFar;
        archive >> fovy;
        archive >> width;
        archive >> height;

        archive >> center;
        archive >> eye;

        SetDirty();
    }
}
