#include "Scene.h"

#include <thread>

// #include "Core/Timer.h"
#include "core/dynamic_variable/common_dvars.h"
#include "core/io/archive.h"
#include "core/systems/job_system.h"

namespace vct {

using ecs::Entity;
using jobsystem::Context;

// @TODO: refactor
#if 1
#define JS_PARALLEL_FOR(CTX, INDEX, COUNT, SUBCOUNT, BODY) \
    CTX.dispatch(                                          \
        static_cast<uint32_t>(COUNT),                      \
        SUBCOUNT,                                          \
        [&](jobsystem::JobArgs args) { const uint32_t INDEX = args.job_index; do BODY while(0); })
#else
#define JS_PARALLEL_FOR(CTX, INDEX, COUNT, SUBCOUNT, BODY) \
    (void)(CTX);                                           \
    for (uint32_t INDEX = 0; INDEX < static_cast<uint32_t>(COUNT); ++INDEX) BODY
#endif

void Scene::update(float dt) {
    m_delta_time = dt;

    Context ctx;

    update_animation(ctx);  // update animation
    ctx.wait();
    update_transformation(ctx);  // update local matrices
    ctx.wait();
    update_hierarchy(ctx);  // update world matrices
    ctx.wait();
    update_armature(ctx);  // update armature matrices
    ctx.wait();

    // update bounding boxes
    // RunObjectUpdateSystem();

    update_camera(ctx);
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
    create<TagComponent>(entity).SetTag(name);
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
                                   float fovy) {
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
    Entity matID = create_material_entity(name + ":mat");
    return create_cube_entity(name, matID, scale, transform);
}

Entity Scene::create_cube_entity(const std::string& name, Entity material_id, const vec3& scale,
                                 const mat4& transform) {
    CRASH_NOW();
    (void)name;
    (void)material_id;
    (void)scale;
    (void)transform;
    Entity entity = create_object_entity(name);
    // TransformComponent& trans = *get_component<TransformComponent>(entity);
    // ObjectComponent& obj = *get_component<ObjectComponent>(entity);
    // trans.MatrixTransform(transform);

    // Entity meshID = Entity_CreateMesh(name + ":mesh");
    // obj.meshID = meshID;

    // MeshComponent& mesh = *get_component<MeshComponent>(meshID);

    // fill_cube_data(scale, mesh);

    // MeshComponent::MeshSubset subset;
    // subset.materialID = materialID;
    // subset.indexCount = static_cast<uint32_t>(mesh.mIndices.size());
    // subset.indexOffset = 0;
    // mesh.mSubsets.emplace_back(subset);

    return entity;
}

void Scene::attach_component(Entity child, Entity parent) {
    DEV_ASSERT(child != parent);
    DEV_ASSERT(parent.is_valid());

    // if child already has a parent, detach it
    if (mHierarchyComponents.contains(child)) {
        CRASH_NOW_MSG("Unlikely to happen at this point");
        detach_component(child);
    }

    HierarchyComponent& hier = mHierarchyComponents.create(child);
    hier.mParent = parent;
}

void Scene::detach_component(Entity entity) {
    unused(entity);
    CRASH_NOW_MSG("TODO");
}

void Scene::Component_DetachChildren(Entity parent) {
    unused(parent);
    CRASH_NOW_MSG("TODO");
}

// static constexpr uint32_t SMALL_SUBTASK_GROUPSIZE = 64;
static constexpr uint32_t SMALL_SUBTASK_GROUPSIZE = 16;

void Scene::update_animation(Context& ctx) {
    JS_PARALLEL_FOR(ctx, i, get_count<AnimationComponent>(), 1, {
        AnimationComponent& animation = mAnimationComponents[i];
        if (!animation.IsPlaying()) {
            continue;
        }

        for (const AnimationComponent::Channel& channel : animation.channels) {
            if (channel.path == AnimationComponent::Channel::Path::UNKNOWN) {
                continue;
            }
            DEV_ASSERT(channel.samplerIndex < (int)animation.samplers.size());
            const AnimationComponent::Sampler& sampler = animation.samplers[channel.samplerIndex];

            int keyLeft = 0;
            int keyRight = 0;
            float timeFirst = std::numeric_limits<float>::min();
            float timeLast = std::numeric_limits<float>::min();
            float timeLeft = std::numeric_limits<float>::min();
            float timeRight = std::numeric_limits<float>::max();

            for (int k = 0; k < (int)sampler.keyframeTimes.size(); ++k) {
                const float time = sampler.keyframeTimes[k];
                if (time < timeFirst) {
                    timeFirst = time;
                }
                if (time > timeLast) {
                    timeLast = time;
                }
                if (time <= animation.timer && time > timeLeft) {
                    timeLeft = time;
                    keyLeft = k;
                }
                if (time >= animation.timer && time < timeRight) {
                    timeRight = time;
                    keyRight = k;
                }
            }

            if (animation.timer < timeFirst) {
                continue;
            }

            const float left = sampler.keyframeTimes[keyLeft];
            const float right = sampler.keyframeTimes[keyRight];

            float t = 0;
            if (keyLeft != keyRight) {
                t = (animation.timer - left) / (right - left);
            }
            t = SaturateF(t);

            TransformComponent* targetTransform = mTransformComponents.get_component(channel.targetID);
            DEV_ASSERT(targetTransform);
            switch (channel.path) {
                case AnimationComponent::Channel::SCALE: {
                    DEV_ASSERT(sampler.keyframeData.size() == sampler.keyframeTimes.size() * 3);
                    const vec3* data = (const vec3*)sampler.keyframeData.data();
                    const vec3& vLeft = data[keyLeft];
                    const vec3& vRight = data[keyRight];
                    targetTransform->set_scale(glm::mix(vLeft, vRight, t));
                    break;
                }
                case AnimationComponent::Channel::TRANSLATION: {
                    DEV_ASSERT(sampler.keyframeData.size() == sampler.keyframeTimes.size() * 3);
                    const vec3* data = (const vec3*)sampler.keyframeData.data();
                    const vec3& vLeft = data[keyLeft];
                    const vec3& vRight = data[keyRight];
                    targetTransform->set_translation(glm::mix(vLeft, vRight, t));
                    break;
                }
                case AnimationComponent::Channel::ROTATION: {
                    DEV_ASSERT(sampler.keyframeData.size() == sampler.keyframeTimes.size() * 4);
                    const vec4* data = (const vec4*)sampler.keyframeData.data();
                    const vec4& vLeft = data[keyLeft];
                    const vec4& vRight = data[keyRight];
                    targetTransform->set_rotation(glm::mix(vLeft, vRight, t));
                    break;
                }
                default:
                    CRASH_NOW();
                    break;
            }
            targetTransform->set_dirty();
        }

        if (animation.IsLooped() && animation.timer > animation.end) {
            animation.timer = animation.start;
        }

        if (animation.IsPlaying()) {
            animation.timer += m_delta_time * animation.speed;
        }
    });
}

void Scene::update_transformation(Context& ctx) {
    JS_PARALLEL_FOR(ctx, i, get_count<TransformComponent>(), SMALL_SUBTASK_GROUPSIZE,
                    { mTransformComponents[i].update_transform(); });
}

void Scene::update_hierarchy(Context& ctx) {
    JS_PARALLEL_FOR(ctx, i, get_count<HierarchyComponent>(), SMALL_SUBTASK_GROUPSIZE, {
        Entity child = mHierarchyComponents.get_entity(i);
        TransformComponent* childTrans = mTransformComponents.get_component(child);
        if (childTrans) {
            const HierarchyComponent* hier = &mHierarchyComponents[i];
            Entity parent = hier->mParent;
            mat4 W = childTrans->get_local_matrix();

            while (parent.is_valid()) {
                TransformComponent* parentTrans = mTransformComponents.get_component(parent);
                if (parentTrans) {
                    W = parentTrans->get_local_matrix() * W;
                }

                if ((hier = mHierarchyComponents.get_component(parent)) != nullptr) {
                    parent = hier->mParent;
                    DEV_ASSERT(parent.is_valid());
                } else {
                    parent.make_invalid();
                }
            }

            childTrans->set_world_matrix(W);
            childTrans->set_dirty(false);
        }
    });
}

void Scene::update_armature(Context& ctx) {
    JS_PARALLEL_FOR(ctx, i, get_count<ArmatureComponent>(), 1, {
        Entity id = mArmatureComponents.get_entity(i);
        ArmatureComponent& armature = mArmatureComponents[i];
        TransformComponent* transform = mTransformComponents.get_component(id);
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
        const size_t numBones = armature.boneCollection.size();
        if (armature.boneTransforms.size() != numBones) {
            armature.boneTransforms.resize(numBones);
        }

        int idx = 0;
        for (Entity boneID : armature.boneCollection) {
            const TransformComponent* boneTransform = mTransformComponents.get_component(boneID);
            DEV_ASSERT(boneTransform);

            const mat4& B = armature.inverseBindMatrices[idx];
            const mat4& W = boneTransform->get_world_matrix();
            const mat4 M = R * W * B;
            armature.boneTransforms[idx] = M;
            ++idx;

            // @TODO: armature animation
        }
    });
}

void Scene::update_camera(Context&) {
    for (int idx = 0; idx < get_count<CameraComponent>(); ++idx) {
        CameraComponent& camera = get_component_array<CameraComponent>()[idx];
        camera.update();
    }
}

void Scene::serialize(Archive& archive) {
    // mGenerator.serialize(archive);
    m_root.serialize(archive);

    mTagComponents.serialize(archive);
    mTransformComponents.serialize(archive);
    mHierarchyComponents.serialize(archive);
    mMaterialComponents.serialize(archive);
    mMeshComponents.serialize(archive);
    mObjectComponents.serialize(archive);
    mCameraComponents.serialize(archive);
    mLightComponents.serialize(archive);
    mArmatureComponents.serialize(archive);
    mAnimationComponents.serialize(archive);
    mRigidBodyPhysicsComponents.serialize(archive);
}

Scene::RayIntersectionResult Scene::Intersects(Ray& ray) {
    RayIntersectionResult result;

    for (int objIdx = 0; objIdx < get_count<ObjectComponent>(); ++objIdx) {
        Entity entity = get_entity<ObjectComponent>(objIdx);
        ObjectComponent& object = get_component_array<ObjectComponent>()[objIdx];
        MeshComponent* mesh = get_component<MeshComponent>(object.meshID);
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