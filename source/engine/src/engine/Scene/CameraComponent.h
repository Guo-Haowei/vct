#pragma once
#include "Math/GeoMath.h"

class Archive;

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
