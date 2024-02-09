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
    if (is_dirty()) {
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

//--------------------------------------------------------------------------------------------------
// Mesh Component
//--------------------------------------------------------------------------------------------------
static size_t get_stride(MeshComponent::VertexAttribute::NAME name) {
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
void vertex_attrib(MeshComponent::VertexAttribute& attrib, const std::vector<T>& buffer, size_t& in_out_offset) {
    attrib.offset_in_byte = (uint32_t)in_out_offset;
    attrib.size_in_byte = (uint32_t)(align(sizeof(T) * buffer.size(), 16llu));
    attrib.stride = (uint32_t)get_stride(attrib.name);
    in_out_offset += attrib.size_in_byte;
}

void MeshComponent::create_render_data() {
    DEV_ASSERT(texcoords_0.size());
    DEV_ASSERT(normals.size());
    // AABB
    local_bound.make_invalid();
    for (MeshSubset& subset : subsets) {
        subset.local_bound.make_invalid();
        for (uint32_t i = 0; i < subset.index_count; ++i) {
            const vec3& point = positions[indices[i + subset.index_offset]];
            subset.local_bound.expand_point(point);
        }
        subset.local_bound.make_valid();
        local_bound.union_box(subset.local_bound);
    }
    // Attributes
    for (int i = 0; i < VertexAttribute::COUNT; ++i) {
        attributes[i].name = static_cast<VertexAttribute::NAME>(i);
    }

    vertex_buffer_size = 0;
    vertex_attrib(attributes[VertexAttribute::POSITION], positions, vertex_buffer_size);
    vertex_attrib(attributes[VertexAttribute::NORMAL], normals, vertex_buffer_size);
    vertex_attrib(attributes[VertexAttribute::TEXCOORD_0], texcoords_0, vertex_buffer_size);
    vertex_attrib(attributes[VertexAttribute::TEXCOORD_1], texcoords_1, vertex_buffer_size);
    vertex_attrib(attributes[VertexAttribute::TANGENT], tangents, vertex_buffer_size);
    vertex_attrib(attributes[VertexAttribute::JOINTS_0], joints_0, vertex_buffer_size);
    vertex_attrib(attributes[VertexAttribute::WEIGHTS_0], weights_0, vertex_buffer_size);
    vertex_attrib(attributes[VertexAttribute::COLOR_0], color_0, vertex_buffer_size);
    return;
}

void MeshComponent::serialize(Archive& archive) {
    CRASH_NOW();
    if (archive.is_write_mode()) {
    } else {
    }
}

std::vector<char> MeshComponent::generate_combined_buffer() const {
    std::vector<char> result;
    result.resize(vertex_buffer_size);

    auto safe_copy = [&](const VertexAttribute& attrib, const void* data) {
        if (attrib.size_in_byte == 0) {
            return;
        }

        memcpy(result.data() + attrib.offset_in_byte, data, attrib.size_in_byte);
        return;
    };
    safe_copy(attributes[VertexAttribute::POSITION], positions.data());
    safe_copy(attributes[VertexAttribute::NORMAL], normals.data());
    safe_copy(attributes[VertexAttribute::TEXCOORD_0], texcoords_0.data());
    safe_copy(attributes[VertexAttribute::TEXCOORD_1], texcoords_1.data());
    safe_copy(attributes[VertexAttribute::TANGENT], tangents.data());
    safe_copy(attributes[VertexAttribute::JOINTS_0], joints_0.data());
    safe_copy(attributes[VertexAttribute::WEIGHTS_0], weights_0.data());
    safe_copy(attributes[VertexAttribute::COLOR_0], color_0.data());
    return result;
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

void TagComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << mTag;
    } else {
        archive >> mTag;
    }
}

}  // namespace vct