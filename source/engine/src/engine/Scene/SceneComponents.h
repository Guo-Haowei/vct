#pragma once
#include "Entity.h"

// #include "Core/Image.h"
#include "Math/AABB.h"
// #include "Graphics/Graphics.h"

class Archive;

class HierarchyComponent {
public:
    ecs::Entity GetParent() const { return mParent; }

    void Serialize(Archive& archive);

private:
    ecs::Entity mParent;

    friend class Scene;
};

struct ObjectComponent {
    enum FLAGS {
        NONE = 0,
        RENDERABLE = 1 << 0,
        CAST_SHADOW = 1 << 1,
        DYNAMIC = 1 << 2,
    };

    uint32_t flags = RENDERABLE | CAST_SHADOW;

    /// mesh
    ecs::Entity meshID;

    void Serialize(Archive& archive);
};

struct ArmatureComponent {
    enum FLAGS {
        NONE = 0,
    };
    uint32_t flags = NONE;

    std::vector<ecs::Entity> boneCollection;
    std::vector<mat4> inverseBindMatrices;

    // Non-Serialized
    std::vector<mat4> boneTransforms;

    void Serialize(Archive& archive);
};

struct AnimationComponent {
    enum : uint32_t {
        NONE = 0,
        PLAYING = 1 << 0,
        LOOPED = 1 << 1,
    };

    inline bool IsPlaying() const { return flags & PLAYING; }
    inline bool IsLooped() const { return flags & LOOPED; }
    inline float GetLength() const { return end - start; }
    inline float IsEnded() const { return timer > end; }

    uint32_t flags = LOOPED;
    float start = 0;
    float end = 0;
    float timer = 0;
    float amount = 1;  // blend amount
    float speed = 1;

    struct Channel {
        ecs::Entity targetID;
        int samplerIndex = -1;

        enum Path {
            TRANSLATION,
            ROTATION,
            SCALE,

            UNKNOWN,
        } path = Path::UNKNOWN;

        enum PathDataType {
            Event,
            Float,
            Float2,
            Float3,
            Float4,
            Weights,

            Count,
        };
    };
    struct Sampler {
        std::vector<float> keyframeTimes;
        std::vector<float> keyframeData;
    };

    std::vector<Channel> channels;
    std::vector<Sampler> samplers;

    void Serialize(Archive& archive);
};

struct RigidBodyPhysicsComponent {
    enum CollisionShape {
        UNKNOWN,
        SPHERE,
        BOX,
    } shape;

    struct BoxParam {
        vec3 halfExtent;
    };
    struct SphereParam {
        float radius;
    };
    union {
        BoxParam box;
        SphereParam sphere;
    } param;
    float mass = 1.0f;

    void Serialize(Archive& archive);
};

#include "CameraComponent.h"
#include "LightComponent.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "TagComponent.h"
#include "TransformComponent.h"
