#pragma once
#include "core/math/geomath.h"

namespace vct {
class Archive;
}
using namespace vct;

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
