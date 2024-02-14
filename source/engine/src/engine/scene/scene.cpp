#include "Scene.h"

#include <thread>

// #include "Core/Timer.h"
#include "core/io/archive.h"
#include "core/systems/job_system.h"

namespace vct {

using ecs::Entity;
using jobsystem::Context;

// static constexpr uint32_t kSmallSubtaskGroupSize = 64;
static constexpr uint32_t kSmallSubtaskGroupSize = 16;

// @TODO: refactor
#if 1
#define JS_PARALLEL_FOR(CTX, INDEX, COUNT, SUBCOUNT, BODY) \
    CTX.dispatch(                                          \
        static_cast<uint32_t>(COUNT),                      \
        SUBCOUNT,                                          \
        [&](jobsystem::JobArgs args) { const uint32_t INDEX = args.job_index; do { BODY; } while(0); })
#else
#define JS_PARALLEL_FOR(CTX, INDEX, COUNT, SUBCOUNT, BODY)                    \
    (void)(CTX);                                                              \
    for (uint32_t INDEX = 0; INDEX < static_cast<uint32_t>(COUNT); ++INDEX) { \
        BODY;                                                                 \
    }
#endif

void Scene::update(float dt) {
    m_delta_time = dt;

    Context ctx;

    // animation
    JS_PARALLEL_FOR(ctx, index, get_count<AnimationComponent>(), 1, update_animation(index));
    ctx.wait();
    // transform
    JS_PARALLEL_FOR(ctx, index, get_count<TransformComponent>(), kSmallSubtaskGroupSize, update_transformation(index));
    ctx.wait();
    // hierarchy
    JS_PARALLEL_FOR(ctx, index, get_count<HierarchyComponent>(), kSmallSubtaskGroupSize, update_hierarchy(index));
    ctx.wait();
    // armature
    JS_PARALLEL_FOR(ctx, index, get_count<ArmatureComponent>(), 1, update_armature(index));
    ctx.wait();

    // update bounding boxes
    // RunObjectUpdateSystem();

    // update camera
    for (int idx = 0; idx < get_count<CameraComponent>(); ++idx) {
        CameraComponent& camera = get_component_array<CameraComponent>()[idx];
        camera.update();
    }
}

void Scene::merge(Scene& other) {
    for (auto& entry : m_component_lib.m_entries) {
        entry.second.m_manager->merge(*other.m_component_lib.m_entries[entry.first].m_manager);
    }
    if (other.m_root.is_valid()) {
        attach_component(other.m_root, m_root);
    }

    m_bound.union_box(other.m_bound);
}

Entity Scene::create_name_entity(const std::string& name) {
    Entity entity = Entity::create();
    create<NameComponent>(entity).set_name(name);
    return entity;
}

Entity Scene::create_transform_entity(const std::string& name) {
    Entity entity = create_name_entity(name);
    create<TransformComponent>(entity);
    return entity;
}

Entity Scene::create_object_entity(const std::string& name) {
    Entity entity = create_name_entity(name);
    create<ObjectComponent>(entity);
    create<TransformComponent>(entity);
    return entity;
}

Entity Scene::create_mesh_entity(const std::string& name) {
    Entity entity = create_name_entity(name);
    create<MeshComponent>(entity);
    return entity;
}

Entity Scene::create_material_entity(const std::string& name) {
    Entity entity = create_name_entity(name);
    create<MaterialComponent>(entity);
    return entity;
}

Entity Scene::create_camera_entity(const std::string& name,
                                   float width,
                                   float height,
                                   float near_plane,
                                   float far_plane,
                                   Degree fovy) {
    Entity entity = create_name_entity(name);
    CameraComponent& camera = create<CameraComponent>(entity);
    camera.m_width = width;
    camera.m_height = height;
    camera.m_near = near_plane;
    camera.m_far = far_plane;
    camera.m_fovy = fovy;
    camera.set_dirty();

    return entity;
}

Entity Scene::create_pointlight_entity(const std::string& name, const vec3& position, const vec3& color,
                                       const float energy) {
    Entity entity = create_name_entity(name);
    (void)name;
    (void)position;
    (void)color;
    (void)energy;
    // TransformComponent& transComponent = create<TransformComponent>(entity);
    // transComponent.SetPosition(position);
    // LightComponent& lightComponent = create<LightComponent>(entity);
    // lightComponent.type = LIGHT_TYPE_POINT;
    // lightComponent.color = color;
    // lightComponent.energy = energy;
    CRASH_NOW();

    return entity;
}

Entity Scene::create_omnilight_entity(const std::string& name, const vec3& color, const float energy) {
    Entity entity = create_name_entity(name);
    create<TransformComponent>(entity);
    LightComponent& light = create<LightComponent>(entity);
    // light.type = LIGHT_TYPE_OMNI;
    light.color = color;
    light.energy = energy;

    return entity;
}

Entity Scene::create_sphere_entity(const std::string& name, float radius, const mat4& transform) {
    Entity matID = create_material_entity(name + ":mat");
    return create_sphere_entity(name, matID, radius, transform);
}

Entity Scene::create_sphere_entity(const std::string& name, Entity material_id, float radius,
                                   const mat4& transform) {
    CRASH_NOW();
    (void)name;
    (void)material_id;
    (void)radius;
    (void)transform;
    Entity entity = create_object_entity(name);
    // TransformComponent& trans = *get_component<TransformComponent>(entity);
    // ObjectComponent& obj = *get_component<ObjectComponent>(entity);
    // trans.MatrixTransform(transform);

    // Entity meshID = Entity_CreateMesh(name + ":mesh");
    // obj.meshID = meshID;

    // MeshComponent& mesh = *get_component<MeshComponent>(meshID);

    // fill_sphere_data(mesh, radius);

    // MeshComponent::MeshSubset subset;
    // subset.materialID = materialID;
    // subset.indexCount = static_cast<uint32_t>(mesh.mIndices.size());
    // subset.indexOffset = 0;
    // mesh.mSubsets.emplace_back(subset);

    return entity;
}

Entity Scene::create_cube_entity(const std::string& name, const vec3& scale, const mat4& transform) {
    Entity material_id = create_material_entity(name + ":mat");
    return create_cube_entity(name, material_id, scale, transform);
}

Entity Scene::create_cube_entity(const std::string& name, Entity material_id, const vec3& scale,
                                 const mat4& transform) {
    ecs::Entity entity = create_object_entity(name);
    TransformComponent& trans = *get_component<TransformComponent>(entity);
    ObjectComponent& obj = *get_component<ObjectComponent>(entity);
    trans.matrix_transform(transform);

    ecs::Entity mesh_id = create_mesh_entity(name + ":mesh");
    obj.mesh_id = mesh_id;

    MeshComponent& mesh = *get_component<MeshComponent>(mesh_id);

    // clang-format off
    constexpr uint32_t indices[] = {
        0,          1,          2,          0,          2,          3,
        0 + 4,      2 + 4,      1 + 4,      0 + 4,      3 + 4,      2 + 4,  // swapped winding
        0 + 4 * 2,  1 + 4 * 2,  2 + 4 * 2,  0 + 4 * 2,  2 + 4 * 2,  3 + 4 * 2,
        0 + 4 * 3,  2 + 4 * 3,  1 + 4 * 3,  0 + 4 * 3,  3 + 4 * 3,  2 + 4 * 3, // swapped winding
        0 + 4 * 4,  2 + 4 * 4,  1 + 4 * 4,  0 + 4 * 4,  3 + 4 * 4,  2 + 4 * 4, // swapped winding
        0 + 4 * 5,  1 + 4 * 5,  2 + 4 * 5,  0 + 4 * 5,  2 + 4 * 5,  3 + 4 * 5,
    };
    // clang-format on

    const vec3& s = scale;
    mesh.positions = {
        // -Z
        vec3(-s.x, +s.y, -s.z),
        vec3(-s.x, -s.y, -s.z),
        vec3(+s.x, -s.y, -s.z),
        vec3(+s.x, +s.y, -s.z),

        // +Z
        vec3(-s.x, +s.y, +s.z),
        vec3(-s.x, -s.y, +s.z),
        vec3(+s.x, -s.y, +s.z),
        vec3(+s.x, +s.y, +s.z),

        // -X
        vec3(-s.x, -s.y, +s.z),
        vec3(-s.x, -s.y, -s.z),
        vec3(-s.x, +s.y, -s.z),
        vec3(-s.x, +s.y, +s.z),

        // +X
        vec3(+s.x, -s.y, +s.z),
        vec3(+s.x, -s.y, -s.z),
        vec3(+s.x, +s.y, -s.z),
        vec3(+s.x, +s.y, +s.z),

        // -Y
        vec3(-s.x, -s.y, +s.z),
        vec3(-s.x, -s.y, -s.z),
        vec3(+s.x, -s.y, -s.z),
        vec3(+s.x, -s.y, +s.z),

        // +Y
        vec3(-s.x, +s.y, +s.z),
        vec3(-s.x, +s.y, -s.z),
        vec3(+s.x, +s.y, -s.z),
        vec3(+s.x, +s.y, +s.z),
    };

    mesh.texcoords_0 = {
        vec2(0, 0),
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),

        vec2(0, 0),
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),

        vec2(0, 0),
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),

        vec2(0, 0),
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),

        vec2(0, 0),
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),

        vec2(0, 0),
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),
    };

    mesh.normals = {
        vec3(0, 0, -1),
        vec3(0, 0, -1),
        vec3(0, 0, -1),
        vec3(0, 0, -1),

        vec3(0, 0, 1),
        vec3(0, 0, 1),
        vec3(0, 0, 1),
        vec3(0, 0, 1),

        vec3(-1, 0, 0),
        vec3(-1, 0, 0),
        vec3(-1, 0, 0),
        vec3(-1, 0, 0),

        vec3(1, 0, 0),
        vec3(1, 0, 0),
        vec3(1, 0, 0),
        vec3(1, 0, 0),

        vec3(0, -1, 0),
        vec3(0, -1, 0),
        vec3(0, -1, 0),
        vec3(0, -1, 0),

        vec3(0, 1, 0),
        vec3(0, 1, 0),
        vec3(0, 1, 0),
        vec3(0, 1, 0),
    };
    MeshComponent::MeshSubset subset;
    subset.material_id = material_id;
    subset.index_count = array_length(indices);
    subset.index_offset = 0;
    mesh.subsets.emplace_back(subset);

    for (int i = 0; i < array_length(indices); i += 3) {
        mesh.indices.push_back(indices[i]);
        mesh.indices.push_back(indices[i + 2]);
        mesh.indices.push_back(indices[i + 1]);
    }

    return entity;
}

void Scene::attach_component(Entity child, Entity parent) {
    DEV_ASSERT(child != parent);
    DEV_ASSERT(parent.is_valid());

    // if child already has a parent, detach it
    if (m_HierarchyComponents.contains(child)) {
        CRASH_NOW_MSG("Unlikely to happen at this point");
        detach_component(child);
    }

    HierarchyComponent& hier = m_HierarchyComponents.create(child);
    hier.m_parent_id = parent;
}

void Scene::detach_component(Entity entity) {
    unused(entity);
    CRASH_NOW_MSG("TODO");
}

void Scene::Component_DetachChildren(Entity parent) {
    unused(parent);
    CRASH_NOW_MSG("TODO");
}

void Scene::update_animation(uint32_t index) {
    AnimationComponent& animation = m_AnimationComponents[index];
    if (!animation.is_playing()) {
        return;
    }

    for (const AnimationComponent::Channel& channel : animation.channels) {
        if (channel.path == AnimationComponent::Channel::PATH_UNKNOWN) {
            continue;
        }
        DEV_ASSERT(channel.sampler_index < (int)animation.samplers.size());
        const AnimationComponent::Sampler& sampler = animation.samplers[channel.sampler_index];

        int key_left = 0;
        int key_right = 0;
        float time_first = std::numeric_limits<float>::min();
        float time_last = std::numeric_limits<float>::min();
        float time_left = std::numeric_limits<float>::min();
        float time_right = std::numeric_limits<float>::max();

        for (int k = 0; k < (int)sampler.keyframe_times.size(); ++k) {
            const float time = sampler.keyframe_times[k];
            if (time < time_first) {
                time_first = time;
            }
            if (time > time_last) {
                time_last = time;
            }
            if (time <= animation.timer && time > time_left) {
                time_left = time;
                key_left = k;
            }
            if (time >= animation.timer && time < time_right) {
                time_right = time;
                key_right = k;
            }
        }

        if (animation.timer < time_first) {
            continue;
        }

        const float left = sampler.keyframe_times[key_left];
        const float right = sampler.keyframe_times[key_right];

        float t = 0;
        if (key_left != key_right) {
            t = (animation.timer - left) / (right - left);
        }
        t = saturate(t);

        TransformComponent* targetTransform = get_component<TransformComponent>(channel.target_id);
        DEV_ASSERT(targetTransform);
        switch (channel.path) {
            case AnimationComponent::Channel::PATH_SCALE: {
                DEV_ASSERT(sampler.keyframe_data.size() == sampler.keyframe_times.size() * 3);
                const vec3* data = (const vec3*)sampler.keyframe_data.data();
                const vec3& vLeft = data[key_left];
                const vec3& vRight = data[key_right];
                targetTransform->set_scale(glm::mix(vLeft, vRight, t));
                break;
            }
            case AnimationComponent::Channel::PATH_TRANSLATION: {
                DEV_ASSERT(sampler.keyframe_data.size() == sampler.keyframe_times.size() * 3);
                const vec3* data = (const vec3*)sampler.keyframe_data.data();
                const vec3& vLeft = data[key_left];
                const vec3& vRight = data[key_right];
                targetTransform->set_translation(glm::mix(vLeft, vRight, t));
                break;
            }
            case AnimationComponent::Channel::PATH_ROTATION: {
                DEV_ASSERT(sampler.keyframe_data.size() == sampler.keyframe_times.size() * 4);
                const vec4* data = (const vec4*)sampler.keyframe_data.data();
                const vec4& vLeft = data[key_left];
                const vec4& vRight = data[key_right];
                targetTransform->set_rotation(glm::mix(vLeft, vRight, t));
                break;
            }
            default:
                CRASH_NOW();
                break;
        }
        targetTransform->set_dirty();
    }

    if (animation.is_looped() && animation.timer > animation.end) {
        animation.timer = animation.start;
    }

    if (animation.is_playing()) {
        // @TODO: set delta time
        animation.timer += m_delta_time * animation.speed;
    }
}

void Scene::update_transformation(uint32_t index) {
    m_TransformComponents[index].update_transform();
}

void Scene::update_hierarchy(uint32_t index) {
    Entity child = get_entity<HierarchyComponent>(index);
    TransformComponent* childTrans = get_component<TransformComponent>(child);
    if (childTrans) {
        const HierarchyComponent* hier = &m_HierarchyComponents[index];
        Entity parent = hier->m_parent_id;
        mat4 W = childTrans->get_local_matrix();

        while (parent.is_valid()) {
            TransformComponent* parentTrans = get_component<TransformComponent>(parent);
            if (parentTrans) {
                W = parentTrans->get_local_matrix() * W;
            }

            if ((hier = get_component<HierarchyComponent>(parent)) != nullptr) {
                parent = hier->m_parent_id;
                DEV_ASSERT(parent.is_valid());
            } else {
                parent.make_invalid();
            }
        }

        childTrans->set_world_matrix(W);
        childTrans->set_dirty(false);
    }
}

void Scene::update_armature(uint32_t index) {
    Entity id = m_ArmatureComponents.get_entity(index);
    ArmatureComponent& armature = m_ArmatureComponents[index];
    TransformComponent* transform = get_component<TransformComponent>(id);
    DEV_ASSERT(transform);

    // The transform world matrices are in world space, but skinning needs them in armature-local space,
    //	so that the skin is reusable for instanced meshes.
    //	We remove the armature's world matrix from the bone world matrix to obtain the bone local transform
    //	These local bone matrices will only be used for skinning, the actual transform components for the bones
    //	remain unchanged.
    //
    //	This is useful for an other thing too:
    //	If a whole transform tree is transformed by some parent (even gltf import does that to convert from RH
    // to LH space) 	then the inverseBindMatrices are not reflected in that because they are not contained in
    // the hierarchy system. 	But this will correct them too.

    const mat4 R = glm::inverse(transform->get_world_matrix());
    const size_t numBones = armature.bone_collection.size();
    if (armature.bone_transforms.size() != numBones) {
        armature.bone_transforms.resize(numBones);
    }

    int idx = 0;
    for (Entity boneID : armature.bone_collection) {
        const TransformComponent* boneTransform = get_component<TransformComponent>(boneID);
        DEV_ASSERT(boneTransform);

        const mat4& B = armature.inverse_bind_matrices[idx];
        const mat4& W = boneTransform->get_world_matrix();
        const mat4 M = R * W * B;
        armature.bone_transforms[idx] = M;
        ++idx;

        // @TODO: armature animation
    }
};

bool Scene::serialize(Archive& archive) {
    // guard and seed
    static const char guard[] = "xScn";
    bool is_read_mode = !archive.is_write_mode();
    if (is_read_mode) {
        std::string read;
        archive >> read;
        if (read != guard) {
            return false;
        }

        uint32_t seed = Entity::MAX_ID;
        archive >> seed;
        Entity::set_seed(seed);
    } else {
        archive << guard;
        archive << Entity::get_seed();
    }

    m_root.serialize(archive);
    if (is_read_mode) {
        archive.read(m_bound);
    } else {
        archive.write(m_bound);
    }

    serialize<NameComponent>(archive);
    serialize<TransformComponent>(archive);
    serialize<HierarchyComponent>(archive);
    serialize<MaterialComponent>(archive);
    serialize<MeshComponent>(archive);
    serialize<ObjectComponent>(archive);
    serialize<CameraComponent>(archive);
    serialize<LightComponent>(archive);
    serialize<ArmatureComponent>(archive);
    serialize<AnimationComponent>(archive);
    serialize<RigidBodyComponent>(archive);

    return true;
}

Scene::RayIntersectionResult Scene::Intersects(Ray& ray) {
    RayIntersectionResult result;

    for (int objIdx = 0; objIdx < get_count<ObjectComponent>(); ++objIdx) {
        Entity entity = get_entity<ObjectComponent>(objIdx);
        ObjectComponent& object = get_component_array<ObjectComponent>()[objIdx];
        MeshComponent* mesh = get_component<MeshComponent>(object.mesh_id);
        TransformComponent* transform = get_component<TransformComponent>(entity);
        DEV_ASSERT(mesh && transform);

        if (!transform || !mesh) {
            continue;
        }

        mat4 inversedModel = glm::inverse(transform->get_world_matrix());
        Ray inversedRay = ray.inverse(inversedModel);
        Ray inversedRayAABB = inversedRay;  // make a copy, we don't want dist to be modified by AABB
        // Perform aabb test
        if (!inversedRayAABB.intersects(mesh->local_bound)) {
            continue;
        }

        // @TODO: test submesh intersection

        // Test every single triange
        for (size_t i = 0; i < mesh->indices.size(); i += 3) {
            const vec3& A = mesh->positions[mesh->indices[i]];
            const vec3& B = mesh->positions[mesh->indices[i + 1]];
            const vec3& C = mesh->positions[mesh->indices[i + 2]];
            if (inversedRay.intersects(A, B, C)) {
                ray.copy_dist(inversedRay);
                result.entity = entity;
                break;
            }
        }
    }

    return result;
}

CameraComponent& Scene::get_main_camera() {
    DEV_ASSERT(get_count<CameraComponent>());
    return get_component_array<CameraComponent>()[0];
}

}  // namespace vct