#include "TransformComponent.h"

#include "Core/Check.h"
#include "engine/Archive.h"

mat4 TransformComponent::GetLocalMatrix() const {
    mat4 rotationMatrix = glm::toMat4(quat(mRotation.w, mRotation.x, mRotation.y, mRotation.z));
    mat4 translationMatrix = glm::translate(mTranslation);
    mat4 scaleMatrix = glm::scale(mScale);
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void TransformComponent::UpdateTransform() {
    if (IsDirty()) {
        SetDirty(false);
        mWorldMatrix = GetLocalMatrix();
    }
}

void TransformComponent::Scale(const vec3& scale) {
    SetDirty();
    mScale.x *= scale.x;
    mScale.y *= scale.y;
    mScale.z *= scale.z;
}

void TransformComponent::Translate(const vec3& translation) {
    SetDirty();
    mTranslation.x += translation.x;
    mTranslation.y += translation.y;
    mTranslation.z += translation.z;
}

void TransformComponent::Rotate(const vec3& euler) {
    unused(euler);
    checkmsg("TODO");
    SetDirty();
}

void TransformComponent::SetLocalTransform(const mat4& matrix) {
    SetDirty();
    Decompose(matrix, mScale, mRotation, mTranslation);
}

void TransformComponent::MatrixTransform(const mat4& matrix) {
    SetDirty();
    Decompose(matrix * GetLocalMatrix(), mScale, mRotation, mTranslation);
}

void TransformComponent::UpdateTransform_Parented(const TransformComponent& parent) {
    unreachable();
    mat4 worldMatrix = GetLocalMatrix();
    const mat4& worldMatrixParent = parent.mWorldMatrix;
    mWorldMatrix = worldMatrixParent * worldMatrix;
}

void TransformComponent::Serialize(Archive& archive) {
    if (archive.IsWriteMode()) {
        archive << mFlags;
        archive << mScale;
        archive << mTranslation;
        archive << mRotation;
    } else {
        archive >> mFlags;
        archive >> mScale;
        archive >> mTranslation;
        archive >> mRotation;
        SetDirty();
    }
}
