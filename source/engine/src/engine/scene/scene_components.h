#pragma once
#include "Entity.h"
#include "core/math/aabb.h"

namespace vct {

class Archive;
class Scene;

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

struct CameraComponent {
    static constexpr float DEFAULT_ZNEAR = 0.1f;
    static constexpr float DEFAULT_ZFAR = 100.0f;
    static constexpr float DEFAULT_FOVY = glm::radians(50.0f);

    enum {
        None = 0,
        Dirty = 1,
    };

    uint32_t flags = Dirty;

    float zNear = DEFAULT_ZFAR;
    float zFar = DEFAULT_ZFAR;
    float fovy = DEFAULT_FOVY;
    float width = 0.0f;
    float height = 0.0f;
    vec3 center = vec3(0);
    vec3 eye = vec3(0, 0, 1);

    // Non-serlialized
    mat4 viewMatrix;
    mat4 projMatrix;

    inline bool IsDirty() const { return flags & Dirty; }
    inline void SetDirty(bool dirty = true) { dirty ? flags |= Dirty : flags &= ~Dirty; }

    mat4 CalculateViewMatrix() const;
    void UpdateCamera();

    void Serialize(Archive& archive);
};

struct LightComponent {
    // LIGHT_TYPE type = LIGHT_TYPE_NONE;
    vec3 color = vec3(1);
    float energy = 10.0f;

    void Serialize(Archive& archive);
};

struct MaterialComponent {
    enum ETextureSlot {
        Base,
        Normal,
        MetallicRoughness,
        Count,
    };

    struct TextureMap {
        std::string name;
    };
    TextureMap mTextures[ETextureSlot::Count];

    float mMetallic = 0.0f;
    float mRoughness = 1.0f;
    vec4 mBaseColor = vec4(1);

    void Serialize(Archive& archive);

    // @TODO: refactor
    mutable void* gpuResource = nullptr;
};

struct MeshComponent {
    enum : uint32_t {
        None = 0,
    };

    struct VertexAttribute {
        enum NAME {
            POSITION = 0,
            NORMAL,
            TEXCOORD_0,
            TEXCOORD_1,
            TANGENT,
            JOINTS_0,
            WEIGHTS_0,
            COLOR_0,
            COUNT,
        } name;

        uint32_t offsetInByte = 0;
        uint32_t sizeInByte = 0;
        uint32_t stride = 0;

        bool IsValid() const { return sizeInByte != 0; }
    };

    struct MeshSubset {
        ecs::Entity materialID;
        uint32_t indexOffset = 0;
        uint32_t indexCount = 0;
        vct::AABB localBound;

        void Serialize(Archive& archive);
    };

    struct GPUBuffers {
        virtual ~GPUBuffers() = default;
    };

    void CreateBounds();
    void CreateRenderData();
    std::vector<char> GenerateCombinedBuffer() const;

    void Serialize(Archive& archive);

    std::vector<uint32_t> mIndices;
    std::vector<vec3> mPositions;
    std::vector<vec3> mNormals;
    std::vector<vec3> mTangents;
    std::vector<vec3> mBitangents;
    std::vector<vec2> mTexcoords_0;
    std::vector<vec2> mTexcoords_1;
    std::vector<ivec4> mJoints_0;
    std::vector<vec4> mWeights_0;
    std::vector<vec3> mColor_0;
    std::vector<MeshSubset> mSubsets;

    ecs::Entity mArmatureID;

    // Non-serialized
    vct::AABB mLocalBound;
    VertexAttribute mAttributes[VertexAttribute::COUNT];
    size_t mVertexBufferSize = 0;  // combine vertex buffer

    // @TODO: refactor
    uint32_t materialIdx = static_cast<uint32_t>(-1);
    mutable void* gpuResource = nullptr;
};

class TagComponent {
public:
    TagComponent() = default;

    TagComponent(const char* tag) { mTag = tag; }

    // bool operator==(const std::string& tag) const { return mTag == tag; }

    void Serialize(Archive& archive);

    void SetTag(const char* tag) { mTag = tag; }
    void SetTag(const std::string& tag) { mTag = tag; }

    const std::string& GetTag() const { return mTag; }
    std::string& GetTagRef() { return mTag; }

private:
    std::string mTag;
};

class TransformComponent {
public:
    enum {
        NONE = 0,
        DIRTY = 1 << 0,
    };

    inline bool IsDirty() const { return mFlags & DIRTY; }
    inline void SetDirty(bool dirty = true) { dirty ? mFlags |= DIRTY : mFlags &= ~DIRTY; }

    inline const vec3& GetPosition() const { return mTranslation; }
    inline void SetPosition(const vec3& t) { mTranslation = t; }

    inline const vec4& GetRotation() const { return mRotation; }
    inline void SetRotation(const vec4& r) { mRotation = r; }

    inline const vec3& GetScale() const { return mScale; }
    inline void SetScale(const vec3& s) { mScale = s; }

    inline const mat4& GetWorldMatrix() const { return mWorldMatrix; }

    void SetWorldMatrix(const mat4& matrix) { mWorldMatrix = matrix; }

    mat4 GetLocalMatrix() const;

    void UpdateTransform();
    void Scale(const vec3& scale);
    void Translate(const vec3& translation);
    void Rotate(const vec3& euler);

    void SetLocalTransform(const mat4& matrix);
    void MatrixTransform(const mat4& matrix);

    void UpdateTransform_Parented(const TransformComponent& parent);

    void Serialize(Archive& archive);

private:
    uint32_t mFlags = DIRTY;

    vec3 mScale = vec3(1);              // local scale
    vec3 mTranslation = vec3(0);        // local translation
    vec4 mRotation = vec4(0, 0, 0, 1);  // local rotation

    // Non-serialized attributes
    mat4 mWorldMatrix = mat4(1);
};

}  // namespace vct