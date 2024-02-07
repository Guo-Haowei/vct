#pragma once
#include "scene_components.h"

#include "core/io/archive.h"

namespace vct {

void HierarchyComponent::Serialize(Archive& archive) { mParent.Serialize(archive); }

void ObjectComponent::Serialize(Archive& archive) { meshID.Serialize(archive); }

void AnimationComponent::Serialize(Archive& archive) {
    unused(archive);
    CRASH_NOW_MSG("NOT IMPLMENTED");
}

void ArmatureComponent::Serialize(Archive& archive) {
    unused(archive);
    CRASH_NOW_MSG("NOT IMPLMENTED");
}

void RigidBodyPhysicsComponent::Serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << shape;
        archive << param;
        archive << mass;
    } else {
        archive >> shape;
        archive >> param;
        archive >> mass;
    }
}

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
    if (archive.is_write_mode()) {
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

void LightComponent::Serialize(Archive& archive) {
    (void)archive;
    // if (archive.is_write_mode())
    // {
    //     archive << type;
    //     archive << color;
    //     archive << energy;
    // }
    // else
    // {
    //     archive >> type;
    //     archive >> color;
    //     archive >> energy;
    // }
}

void MaterialComponent::Serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << mMetallic;
        archive << mRoughness;
        archive << mBaseColor;
    } else {
        archive >> mMetallic;
        archive >> mRoughness;
        archive >> mBaseColor;
    }
}

static uint32_t GetStride(MeshComponent::VertexAttribute::NAME name) {
    switch (name) {
        case MeshComponent::VertexAttribute::POSITION:
        case MeshComponent::VertexAttribute::NORMAL:
        case MeshComponent::VertexAttribute::TANGENT:
            return sizeof(vec3);
        case MeshComponent::VertexAttribute::TEXCOORD_0:
        case MeshComponent::VertexAttribute::TEXCOORD_1:
            return sizeof(vec2);
        case MeshComponent::VertexAttribute::JOINTS_0:
            return sizeof(ivec4);
        case MeshComponent::VertexAttribute::COLOR_0:
        case MeshComponent::VertexAttribute::WEIGHTS_0:
            return sizeof(vec4);
        default:
            CRASH_NOW();
            return 0;
    }
}

template<typename T>
void VertexAttribHelper(MeshComponent::VertexAttribute& attrib, const std::vector<T>& buffer, size_t& outOffset) {
    attrib.offsetInByte = static_cast<uint32_t>(outOffset);
    attrib.sizeInByte = (uint32_t)(align(sizeof(T) * buffer.size(), 16llu));
    attrib.stride = GetStride(attrib.name);
    outOffset += attrib.sizeInByte;
}

void MeshComponent::CreateBounds() {
    mLocalBound.make_invalid();
    for (MeshSubset& subset : mSubsets) {
        subset.localBound.make_invalid();
        for (uint32_t i = 0; i < subset.indexCount; ++i) {
            const vec3& point = mPositions[mIndices[i + subset.indexOffset]];
            subset.localBound.expand_point(point);
        }
        subset.localBound.make_valid();
        mLocalBound.union_box(subset.localBound);
    }
}

void MeshComponent::CreateRenderData() {
    DEV_ASSERT(mTexcoords_0.size());
    DEV_ASSERT(mNormals.size());
    // AABB
    CreateBounds();

    // Attributes
    for (int i = 0; i < VertexAttribute::COUNT; ++i) {
        mAttributes[i].name = static_cast<VertexAttribute::NAME>(i);
    }

    mVertexBufferSize = 0;
    VertexAttribHelper(mAttributes[VertexAttribute::POSITION], mPositions, mVertexBufferSize);
    VertexAttribHelper(mAttributes[VertexAttribute::NORMAL], mNormals, mVertexBufferSize);
    VertexAttribHelper(mAttributes[VertexAttribute::TEXCOORD_0], mTexcoords_0, mVertexBufferSize);
    VertexAttribHelper(mAttributes[VertexAttribute::TEXCOORD_1], mTexcoords_1, mVertexBufferSize);
    VertexAttribHelper(mAttributes[VertexAttribute::TANGENT], mTangents, mVertexBufferSize);
    VertexAttribHelper(mAttributes[VertexAttribute::JOINTS_0], mJoints_0, mVertexBufferSize);
    VertexAttribHelper(mAttributes[VertexAttribute::WEIGHTS_0], mWeights_0, mVertexBufferSize);
    VertexAttribHelper(mAttributes[VertexAttribute::COLOR_0], mColor_0, mVertexBufferSize);
    return;
}

std::vector<char> MeshComponent::GenerateCombinedBuffer() const {
    std::vector<char> result;
    result.resize(mVertexBufferSize);

    auto SafeCopy = [&](const VertexAttribute& attrib, const void* data) {
        if (attrib.sizeInByte == 0) {
            return;
        }

        memcpy(result.data() + attrib.offsetInByte, data, attrib.sizeInByte);
        return;
    };
    SafeCopy(mAttributes[VertexAttribute::POSITION], mPositions.data());
    SafeCopy(mAttributes[VertexAttribute::NORMAL], mNormals.data());
    SafeCopy(mAttributes[VertexAttribute::TEXCOORD_0], mTexcoords_0.data());
    SafeCopy(mAttributes[VertexAttribute::TEXCOORD_1], mTexcoords_1.data());
    SafeCopy(mAttributes[VertexAttribute::TANGENT], mTangents.data());
    SafeCopy(mAttributes[VertexAttribute::JOINTS_0], mJoints_0.data());
    SafeCopy(mAttributes[VertexAttribute::WEIGHTS_0], mWeights_0.data());
    SafeCopy(mAttributes[VertexAttribute::COLOR_0], mColor_0.data());
    return result;
}

void MeshComponent::MeshSubset::Serialize(Archive& archive) {
    materialID.Serialize(archive);
    if (archive.is_write_mode()) {
        archive << indexOffset;
        archive << indexCount;
        archive.write(&localBound, sizeof(AABB));
    } else {
        archive >> indexOffset;
        archive >> indexCount;
        archive.read(&localBound, sizeof(AABB));
    }
}

void MeshComponent::Serialize(Archive& archive) {
#define SERIALIZE_MESH(OP)   \
    archive OP mIndices;     \
    archive OP mPositions;   \
    archive OP mNormals;     \
    archive OP mTangents;    \
    archive OP mTexcoords_0; \
    archive OP mTexcoords_1; \
    archive OP mJoints_0;    \
    archive OP mWeights_0;   \
    archive OP mColor_0;

#define SERIALIZE()   SERIALIZE_MESH(<<)
#define DESERIALIZE() SERIALIZE_MESH(>>)

    CRASH_NOW_MSG("TODO");
    // if (archive.is_write_mode()) {
    //     SERIALIZE();

    //    size_t size = mSubsets.size();
    //    archive << size;
    //    for (auto& subset : mSubsets) {
    //        subset.Serialize(archive);
    //    }
    //} else {
    //    DESERIALIZE();

    //    size_t size = 0;
    //    archive >> size;
    //    mSubsets.resize(size);
    //    for (size_t i = 0; i < size; ++i) {
    //        mSubsets[i].Serialize(archive);
    //    }
    //}

    // mArmatureID.Serialize(archive);
}

void TagComponent::Serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << mTag;
    } else {
        archive >> mTag;
    }
}

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
    CRASH_NOW_MSG("TODO");
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
    CRASH_NOW();
    mat4 worldMatrix = GetLocalMatrix();
    const mat4& worldMatrixParent = parent.mWorldMatrix;
    mWorldMatrix = worldMatrixParent * worldMatrix;
}

void TransformComponent::Serialize(Archive& archive) {
    if (archive.is_write_mode()) {
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

}  // namespace vct