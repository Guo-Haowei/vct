#pragma once
#include "Entity.h"
#include "Math/AABB.h"

class Archive;

struct MeshComponent
{
    enum : uint32_t
    {
        None = 0,
    };

    struct VertexAttribute
    {
        enum NAME
        {
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

    struct MeshSubset
    {
        ecs::Entity materialID;
        uint32_t indexOffset = 0;
        uint32_t indexCount = 0;
        AABB localBound;

        void Serialize(Archive& archive);
    };

    struct GPUBuffers
    {
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
    AABB mLocalBound;
    VertexAttribute mAttributes[VertexAttribute::COUNT];
    size_t mVertexBufferSize = 0;  // combine vertex buffer

    // @TODO: refactor
    uint32_t materialIdx = static_cast<uint32_t>(-1);
    mutable void* gpuResource = nullptr;
};
