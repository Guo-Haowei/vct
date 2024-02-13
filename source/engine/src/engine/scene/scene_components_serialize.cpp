#include "assets/asset_loader.h"
#include "core/io/archive.h"
#include "scene_components.h"

namespace vct {

// @TODO: version control
[[maybe_unused]] static inline uint32_t VERSION = 1;

void NameComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << m_name;
    } else {
        archive >> m_name;
    }
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

void HierarchyComponent::serialize(Archive& archive) {
    m_parent_id.serialize(archive);
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

void MeshComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << flags;
        archive << indices;
        archive << positions;
        archive << normals;
        archive << tangents;
        archive << texcoords_0;
        archive << texcoords_1;
        archive << joints_0;
        archive << weights_0;
        archive << color_0;
        archive << subsets;
        archive << armature_id;
    } else {
        archive >> flags;
        archive >> indices;
        archive >> positions;
        archive >> normals;
        archive >> tangents;
        archive >> texcoords_0;
        archive >> texcoords_1;
        archive >> joints_0;
        archive >> weights_0;
        archive >> color_0;
        archive >> subsets;
        archive >> armature_id;

        create_render_data();
    }
}

void MaterialComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << metallic;
        archive << roughness;
        archive << base_color;
        for (int i = 0; i < TEXTURE_MAX; ++i) {
            archive << textures[i].name;
        }
    } else {
        archive >> metallic;
        archive >> roughness;
        archive >> base_color;
        for (int i = 0; i < TEXTURE_MAX; ++i) {
            std::string& texture = textures[i].name;
            archive >> texture;
            if (!texture.empty()) {
                asset_loader::load_image_sync(textures[i].name);
            }
        }
    }

    // @TODO: request image
}

void LightComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << type;
        archive << color;
        archive << energy;
    } else {
        archive >> type;
        archive >> color;
        archive >> energy;
    }
}

void ObjectComponent::serialize(Archive& archive) {
    mesh_id.serialize(archive);
}

void AnimationComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << flags;
        archive << start;
        archive << end;
        archive << timer;
        archive << amount;
        archive << speed;
        archive << channels;

        uint64_t num_samplers = samplers.size();
        archive << num_samplers;
        for (uint64_t i = 0; i < num_samplers; ++i) {
            archive << samplers[i].keyframe_times;
            archive << samplers[i].keyframe_data;
        }
    } else {
        archive >> flags;
        archive >> start;
        archive >> end;
        archive >> timer;
        archive >> amount;
        archive >> speed;
        archive >> channels;

        uint64_t num_samplers = 0;
        archive >> num_samplers;
        samplers.resize(num_samplers);
        for (uint64_t i = 0; i < num_samplers; ++i) {
            archive >> samplers[i].keyframe_times;
            archive >> samplers[i].keyframe_data;
        }
    }
}

void ArmatureComponent::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << flags;
        archive << bone_collection;
        archive << inverse_bind_matrices;
    } else {
        archive >> flags;
        archive >> bone_collection;
        archive >> inverse_bind_matrices;
    }
}

void RigidBodyComponent::serialize(Archive& archive) {
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

}  // namespace vct
