#include "Scene.h"

#include <thread>

#include "Archive.h"
#include "Core/CommonDvars.h"
#include "Core/JobSystem.h"
#include "Core/Timer.h"
// #include "Math/Primitives.h"

using jobsystem::Context;

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

void Scene::Update(float dt) {
    mDeltaTime = dt;

    Context ctx;

    RunAnimationUpdateSystem(ctx);  // update animation
    ctx.Wait();
    RunTransformUpdateSystem(ctx);  // update local matrices
    ctx.Wait();
    RunHierarchyUpdateSystem(ctx);  // update world matrices
    ctx.Wait();
    RunArmatureUpdateSystem(ctx);  // update armature matrices
    ctx.Wait();

    // update bounding boxes
    // RunObjectUpdateSystem();

    RunCameraUpdateSystem();

    RunLightUpdateSystem();
}

void Scene::Merge(Scene& other) {
    unused(other);
    CRASH_NOW_MSG("FIX");
}

ecs::Entity Scene::Entity_CreateName(const std::string& name) {
    ecs::Entity entity = mGenerator.Create();
    Create<TagComponent>(entity).SetTag(name);
    return entity;
}

ecs::Entity Scene::Entity_CreateTransform(const std::string& name) {
    ecs::Entity entity = Entity_CreateName(name);
    Create<TransformComponent>(entity);
    return entity;
}

ecs::Entity Scene::Entity_CreateObject(const std::string& name) {
    ecs::Entity entity = Entity_CreateName(name);
    Create<ObjectComponent>(entity);
    Create<TransformComponent>(entity);
    return entity;
}

ecs::Entity Scene::Entity_CreateMesh(const std::string& name) {
    ecs::Entity entity = Entity_CreateName(name);
    Create<MeshComponent>(entity);
    return entity;
}

ecs::Entity Scene::Entity_CreateMaterial(const std::string& name) {
    ecs::Entity entity = Entity_CreateName(name);
    Create<MaterialComponent>(entity);
    return entity;
}

ecs::Entity Scene::Entity_CreateCamera(const std::string& name, float width, float height, float near_plane,
                                       float far_plane, float fovy) {
    ecs::Entity entity = Entity_CreateName(name);
    CameraComponent& cameraComponent = Create<CameraComponent>(entity);
    cameraComponent.width = width;
    cameraComponent.height = height;
    cameraComponent.zNear = near_plane;
    cameraComponent.zFar = far_plane;
    cameraComponent.fovy = fovy;

    return entity;
}

ecs::Entity Scene::Entity_CreatePointLight(const std::string& name, const vec3& position, const vec3& color,
                                           const float energy) {
    ecs::Entity entity = Entity_CreateName(name);
    (void)name;
    (void)position;
    (void)color;
    (void)energy;
    // TransformComponent& transComponent = Create<TransformComponent>(entity);
    // transComponent.SetPosition(position);
    // LightComponent& lightComponent = Create<LightComponent>(entity);
    // lightComponent.type = LIGHT_TYPE_POINT;
    // lightComponent.color = color;
    // lightComponent.energy = energy;

    return entity;
}

ecs::Entity Scene::Entity_CreateOmniLight(const std::string& name, const vec3& color, const float energy) {
    ecs::Entity entity = Entity_CreateName(name);
    (void)color;
    (void)energy;
    // Create<TransformComponent>(entity);
    // LightComponent& lightComponent = Create<LightComponent>(entity);
    // lightComponent.type = LIGHT_TYPE_OMNI;
    // lightComponent.color = color;
    // lightComponent.energy = energy;

    return entity;
}

ecs::Entity Scene::Entity_CreateSphere(const std::string& name, float radius, const mat4& transform) {
    ecs::Entity matID = Entity_CreateMaterial(name + ":mat");
    return Entity_CreateSphere(name, matID, radius, transform);
}

ecs::Entity Scene::Entity_CreateSphere(const std::string& name, ecs::Entity materialID, float radius,
                                       const mat4& transform) {
    (void)name;
    (void)materialID;
    (void)radius;
    (void)transform;
    ecs::Entity entity = Entity_CreateObject(name);
    // TransformComponent& trans = *GetComponent<TransformComponent>(entity);
    // ObjectComponent& obj = *GetComponent<ObjectComponent>(entity);
    // trans.MatrixTransform(transform);

    // ecs::Entity meshID = Entity_CreateMesh(name + ":mesh");
    // obj.meshID = meshID;

    // MeshComponent& mesh = *GetComponent<MeshComponent>(meshID);

    // fill_sphere_data(mesh, radius);

    // MeshComponent::MeshSubset subset;
    // subset.materialID = materialID;
    // subset.indexCount = static_cast<uint32_t>(mesh.mIndices.size());
    // subset.indexOffset = 0;
    // mesh.mSubsets.emplace_back(subset);

    return entity;
}

ecs::Entity Scene::Entity_CreateCube(const std::string& name, const vec3& scale, const mat4& transform) {
    ecs::Entity matID = Entity_CreateMaterial(name + ":mat");
    return Entity_CreateCube(name, matID, scale, transform);
}

ecs::Entity Scene::Entity_CreateCube(const std::string& name, ecs::Entity materialID, const vec3& scale,
                                     const mat4& transform) {
    (void)name;
    (void)materialID;
    (void)scale;
    (void)transform;
    ecs::Entity entity = Entity_CreateObject(name);
    // TransformComponent& trans = *GetComponent<TransformComponent>(entity);
    // ObjectComponent& obj = *GetComponent<ObjectComponent>(entity);
    // trans.MatrixTransform(transform);

    // ecs::Entity meshID = Entity_CreateMesh(name + ":mesh");
    // obj.meshID = meshID;

    // MeshComponent& mesh = *GetComponent<MeshComponent>(meshID);

    // fill_cube_data(scale, mesh);

    // MeshComponent::MeshSubset subset;
    // subset.materialID = materialID;
    // subset.indexCount = static_cast<uint32_t>(mesh.mIndices.size());
    // subset.indexOffset = 0;
    // mesh.mSubsets.emplace_back(subset);

    return entity;
}

void Scene::Component_Attach(ecs::Entity child, ecs::Entity parent) {
    DEV_ASSERT(child != parent);
    DEV_ASSERT(parent.IsValid());

    // if child already has a parent, detach it
    if (mHierarchyComponents.Contains(child)) {
        CRASH_NOW_MSG("Unlikely to happen at this point");
        Component_Detach(child);
    }

    HierarchyComponent& hier = mHierarchyComponents.Create(child);
    hier.mParent = parent;
}

void Scene::Component_Detach(ecs::Entity entity) {
    unused(entity);
    CRASH_NOW_MSG("TODO");
}

void Scene::Component_DetachChildren(ecs::Entity parent) {
    unused(parent);
    CRASH_NOW_MSG("TODO");
}

// static constexpr uint32_t SMALL_SUBTASK_GROUPSIZE = 64;
static constexpr uint32_t SMALL_SUBTASK_GROUPSIZE = 16;

void Scene::RunAnimationUpdateSystem(Context& ctx) {
    JS_PARALLEL_FOR(ctx, i, GetCount<AnimationComponent>(), 1, {
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

            TransformComponent* targetTransform = mTransformComponents.GetComponent(channel.targetID);
            DEV_ASSERT(targetTransform);
            switch (channel.path) {
                case AnimationComponent::Channel::SCALE: {
                    DEV_ASSERT(sampler.keyframeData.size() == sampler.keyframeTimes.size() * 3);
                    const vec3* data = (const vec3*)sampler.keyframeData.data();
                    const vec3& vLeft = data[keyLeft];
                    const vec3& vRight = data[keyRight];
                    targetTransform->SetScale(glm::mix(vLeft, vRight, t));
                    break;
                }
                case AnimationComponent::Channel::TRANSLATION: {
                    DEV_ASSERT(sampler.keyframeData.size() == sampler.keyframeTimes.size() * 3);
                    const vec3* data = (const vec3*)sampler.keyframeData.data();
                    const vec3& vLeft = data[keyLeft];
                    const vec3& vRight = data[keyRight];
                    targetTransform->SetPosition(glm::mix(vLeft, vRight, t));
                    break;
                }
                case AnimationComponent::Channel::ROTATION: {
                    DEV_ASSERT(sampler.keyframeData.size() == sampler.keyframeTimes.size() * 4);
                    const vec4* data = (const vec4*)sampler.keyframeData.data();
                    const vec4& vLeft = data[keyLeft];
                    const vec4& vRight = data[keyRight];
                    targetTransform->SetRotation(glm::mix(vLeft, vRight, t));
                    break;
                }
                default:
                    CRASH_NOW();
                    break;
            }
            targetTransform->SetDirty();
        }

        if (animation.IsLooped() && animation.timer > animation.end) {
            animation.timer = animation.start;
        }

        if (animation.IsPlaying()) {
            animation.timer += mDeltaTime * animation.speed;
        }
    });
}

void Scene::RunTransformUpdateSystem(Context& ctx) {
    JS_PARALLEL_FOR(ctx, i, GetCount<TransformComponent>(), SMALL_SUBTASK_GROUPSIZE,
                    { mTransformComponents[i].UpdateTransform(); });
}

void Scene::RunHierarchyUpdateSystem(Context& ctx) {
    JS_PARALLEL_FOR(ctx, i, GetCount<HierarchyComponent>(), SMALL_SUBTASK_GROUPSIZE, {
        ecs::Entity child = mHierarchyComponents.GetEntity(i);
        TransformComponent* childTrans = mTransformComponents.GetComponent(child);
        if (childTrans) {
            const HierarchyComponent* hier = &mHierarchyComponents[i];
            ecs::Entity parent = hier->mParent;
            mat4 W = childTrans->GetLocalMatrix();

            while (parent.IsValid()) {
                TransformComponent* parentTrans = mTransformComponents.GetComponent(parent);
                if (parentTrans) {
                    W = parentTrans->GetLocalMatrix() * W;
                }

                if ((hier = mHierarchyComponents.GetComponent(parent)) != nullptr) {
                    parent = hier->mParent;
                    DEV_ASSERT(parent.IsValid());
                } else {
                    parent = ecs::Entity::INVALID;
                }
            }

            childTrans->SetWorldMatrix(W);
            childTrans->SetDirty(false);
        }
    });
}

void Scene::RunArmatureUpdateSystem(Context& ctx) {
    JS_PARALLEL_FOR(ctx, i, GetCount<ArmatureComponent>(), 1, {
        ecs::Entity id = mArmatureComponents.GetEntity(i);
        ArmatureComponent& armature = mArmatureComponents[i];
        TransformComponent* transform = mTransformComponents.GetComponent(id);
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

        const mat4 R = glm::inverse(transform->GetWorldMatrix());
        const size_t numBones = armature.boneCollection.size();
        if (armature.boneTransforms.size() != numBones) {
            armature.boneTransforms.resize(numBones);
        }

        int idx = 0;
        for (ecs::Entity boneID : armature.boneCollection) {
            const TransformComponent* boneTransform = mTransformComponents.GetComponent(boneID);
            DEV_ASSERT(boneTransform);

            const mat4& B = armature.inverseBindMatrices[idx];
            const mat4& W = boneTransform->GetWorldMatrix();
            const mat4 M = R * W * B;
            armature.boneTransforms[idx] = M;
            ++idx;

            // @TODO: armature animation
        }
    });
}

void Scene::RunObjectUpdateSystem() { CRASH_NOW(); }

void Scene::RunCameraUpdateSystem() {
    for (int i = 0; i < GetCount<CameraComponent>(); ++i) {
        ecs::Entity cam = GetEntity<CameraComponent>(i);
        CameraComponent& cameraComponent = mCameraComponents[i];
        cameraComponent.UpdateCamera();
    }
}

void Scene::RunLightUpdateSystem() {}

void Scene::Serialize(Archive& archive) {
    mGenerator.Serialize(archive);
    mRoot.Serialize(archive);

    mTagComponents.Serialize(archive);
    mTransformComponents.Serialize(archive);
    mHierarchyComponents.Serialize(archive);
    mMaterialComponents.Serialize(archive);
    mMeshComponents.Serialize(archive);
    mObjectComponents.Serialize(archive);
    mCameraComponents.Serialize(archive);
    mLightComponents.Serialize(archive);
    mArmatureComponents.Serialize(archive);
    mAnimationComponents.Serialize(archive);
    mRigidBodyPhysicsComponents.Serialize(archive);
}

Scene::RayIntersectionResult Scene::Intersects(Ray& ray) {
    RayIntersectionResult result;

    for (int objIdx = 0; objIdx < GetCount<ObjectComponent>(); ++objIdx) {
        ecs::Entity entity = GetEntity<ObjectComponent>(objIdx);
        ObjectComponent& object = GetComponentArray<ObjectComponent>()[objIdx];
        MeshComponent* mesh = GetComponent<MeshComponent>(object.meshID);
        TransformComponent* transform = GetComponent<TransformComponent>(entity);
        DEV_ASSERT(mesh && transform);

        if (!transform || !mesh) {
            continue;
        }

        mat4 inversedModel = glm::inverse(transform->GetWorldMatrix());
        Ray inversedRay = ray.Inverse(inversedModel);
        Ray inversedRayAABB = inversedRay;  // make a copy, we don't want dist to be modified by AABB
        // Perform aabb test
        if (!inversedRayAABB.Intersects(mesh->mLocalBound)) {
            continue;
        }

        // @TODO: test submesh intersection

        // Test every single triange
        for (size_t i = 0; i < mesh->mIndices.size(); i += 3) {
            const vec3& A = mesh->mPositions[mesh->mIndices[i]];
            const vec3& B = mesh->mPositions[mesh->mIndices[i + 1]];
            const vec3& C = mesh->mPositions[mesh->mIndices[i + 2]];
            if (inversedRay.Intersects(A, B, C)) {
                ray.CopyDist(inversedRay);
                result.entity = entity;
                break;
            }
        }
    }

    return result;
}