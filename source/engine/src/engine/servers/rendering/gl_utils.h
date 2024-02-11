#pragma once
#include "GpuTexture.h"
#include "core/math/geomath.h"

struct MeshData {
    GLuint vao = 0;
    GLuint ebo = 0;
    GLuint vbos[6] = { 0 };
    uint32_t count = 0;
};

struct MaterialData {
    GpuTexture albedoMap;
    GpuTexture materialMap;
    GpuTexture normalMap;
    vec4 albedoColor;
    float metallic;
    float roughness;
    float reflectPower;
    int textureMapIdx;
};

void R_CreateQuad();
void R_DrawQuad();

namespace gl {

//------------------------------------------------------------------------------
// Constant Buffer
//------------------------------------------------------------------------------

GLuint CreateAndBindConstantBuffer(int slot, size_t size_in_byte);
void UpdateConstantBuffer(GLuint handle, const void* ptr, size_t size_in_byte);

template<typename T>
struct ConstantBuffer {

    void Destroy() {
        if (mHandle != 0) {
            LOG_VERBOSE("[opengl] destroy cbuffer {}", mHandle);
            glDeleteBuffers(1, &mHandle);
        }
        mHandle = 0;
    }

    void CreateAndBind() {
        static_assert(sizeof(T) % 16 == 0);
        mHandle = CreateAndBindConstantBuffer(cache.get_slot(), sizeof(T));
    }

    void Update() { UpdateConstantBuffer(mHandle, &cache, sizeof(T)); }

    T cache;
    GLuint mHandle = 0;
};

static inline GLuint64 MakeTextureResident(GLuint texture) {
    GLuint64 ret = glGetTextureHandleARB(texture);
    glMakeTextureHandleResidentARB(ret);
    return ret;
}

}  // namespace gl
