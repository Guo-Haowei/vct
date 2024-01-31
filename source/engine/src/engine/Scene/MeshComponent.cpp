#include "MeshComponent.h"

#include "Archive.h"

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
    attrib.sizeInByte = (uint32_t)(ALIGN(sizeof(T) * buffer.size(), 16));
    attrib.stride = GetStride(attrib.name);
    outOffset += attrib.sizeInByte;
}

void MeshComponent::CreateBounds() {
    mLocalBound.MakeInvalid();
    for (MeshSubset& subset : mSubsets) {
        subset.localBound.MakeInvalid();
        for (uint32_t i = 0; i < subset.indexCount; ++i) {
            const vec3& point = mPositions[mIndices[i + subset.indexOffset]];
            subset.localBound.Expand(point);
        }
        subset.localBound.MakeValid();
        mLocalBound.Union(subset.localBound);
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
    if (archive.IsWriteMode()) {
        archive << indexOffset;
        archive << indexCount;
        archive.Write(&localBound, sizeof(AABB));
    } else {
        archive >> indexOffset;
        archive >> indexCount;
        archive.Read(&localBound, sizeof(AABB));
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

    if (archive.IsWriteMode()) {
        SERIALIZE();

        size_t size = mSubsets.size();
        archive << size;
        for (auto& subset : mSubsets) {
            subset.Serialize(archive);
        }
    } else {
        DESERIALIZE();

        size_t size = 0;
        archive >> size;
        mSubsets.resize(size);
        for (size_t i = 0; i < size; ++i) {
            mSubsets[i].Serialize(archive);
        }
    }

    mArmatureID.Serialize(archive);
}
