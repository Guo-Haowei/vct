#pragma once
#include "scene_components.h"

#include "core/io/archive.h"

namespace vct {

//--------------------------------------------------------------------------------------------------
// TransformComponent
//--------------------------------------------------------------------------------------------------
mat4 TransformComponent::get_local_matrix() const {
    mat4 rotationMatrix = glm::toMat4(quat(m_rotation.w, m_rotation.x, m_rotation.y, m_rotation.z));
    mat4 translationMatrix = glm::translate(m_translation);
    mat4 scaleMatrix = glm::scale(m_scale);
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void TransformComponent::update_transform() {
    if (is_dirty()) {
        set_dirty(false);
        m_world_matrix = get_local_matrix();
    }
}

void TransformComponent::scale(const vec3& scale) {
    set_dirty();
    m_scale.x *= scale.x;
    m_scale.y *= scale.y;
    m_scale.z *= scale.z;
}

void TransformComponent::translate(const vec3& translation) {
    set_dirty();
    m_translation.x += translation.x;
    m_translation.y += translation.y;
    m_translation.z += translation.z;
}

void TransformComponent::rotate(const vec3& euler) {
    set_dirty();
    glm::quat quaternion(m_rotation.w, m_rotation.x, m_rotation.y, m_rotation.z);
    quaternion = glm::quat(euler) * quaternion;

    m_rotation.x = quaternion.x;
    m_rotation.y = quaternion.y;
    m_rotation.z = quaternion.z;
    m_rotation.w = quaternion.w;
}

void TransformComponent::set_local_transform(const mat4& matrix) {
    set_dirty();
    Decompose(matrix, m_scale, m_rotation, m_translation);
}

void TransformComponent::matrix_transform(const mat4& matrix) {
    set_dirty();
    Decompose(matrix * get_local_matrix(), m_scale, m_rotation, m_translation);
}

void TransformComponent::update_transform_parented(const TransformComponent& parent) {
    CRASH_NOW();
    mat4 worldMatrix = get_local_matrix();
    const mat4& worldMatrixParent = parent.m_world_matrix;
    m_world_matrix = worldMatrixParent * worldMatrix;
}

void TransformComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << m_flags;
        archive << m_scale;
        archive << m_translation;
        archive << m_rotation;
    } else {
        archive >> m_flags;
        archive >> m_scale;
        archive >> m_translation;
        archive >> m_rotation;
        set_dirty();
    }
}

//--------------------------------------------------------------------------------------------------
// CameraComponent
//--------------------------------------------------------------------------------------------------
void CameraComponent::update() {
    // @TODO: check size
    // if (is_dirty())
    {
        const float aspect = m_width / m_height;
        m_view_matrix = glm::lookAt(m_eye, m_center, vec3(0, 1, 0));
        m_projection_matrix = glm::perspective(m_fovy, aspect, m_near, m_far);
        m_projection_view_matrix = m_projection_matrix * m_view_matrix;
        set_dirty(false);
    }
}

void CameraComponent::set_dimension(float width, float height) {
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
        set_dirty();
    }
}

void CameraComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << m_flags;
        archive << m_near;
        archive << m_far;
        archive << m_fovy;
        archive << m_width;
        archive << m_height;
    } else {
        archive >> m_flags;
        archive >> m_near;
        archive >> m_far;
        archive >> m_fovy;
        archive >> m_width;
        archive >> m_height;

        set_dirty();
    }
}

void HierarchyComponent::serialize(Archive& archive) { mParent.serialize(archive); }

void ObjectComponent::serialize(Archive& archive) { meshID.serialize(archive); }

void AnimationComponent::serialize(Archive& archive) {
    unused(archive);
    CRASH_NOW_MSG("NOT IMPLMENTED");
}

void ArmatureComponent::serialize(Archive& archive) {
    unused(archive);
    CRASH_NOW_MSG("NOT IMPLMENTED");
}

void RigidBodyPhysicsComponent::serialize(Archive& archive) {
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

void LightComponent::serialize(Archive& archive) {
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

void MaterialComponent::serialize(Archive& archive) {
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

void MeshComponent::MeshSubset::serialize(Archive& archive) {
    materialID.serialize(archive);
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

void MeshComponent::serialize(Archive& archive) {
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
    //        subset.serialize(archive);
    //    }
    //} else {
    //    DESERIALIZE();

    //    size_t size = 0;
    //    archive >> size;
    //    mSubsets.resize(size);
    //    for (size_t i = 0; i < size; ++i) {
    //        mSubsets[i].serialize(archive);
    //    }
    //}

    // mArmatureID.serialize(archive);
}

void TagComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << mTag;
    } else {
        archive >> mTag;
    }
}

}  // namespace vct