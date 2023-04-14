#pragma once
#include "Intersection.h"

class AABB
{
public:
    AABB(const vec3& min, const vec3& max);
    AABB() { MakeInvalid(); }

    void MakeInvalid();

    inline bool IsValid() const
    {
        return (mMin.x < mMax.x) && (mMin.y < mMax.y) && (mMin.z < mMax.z);
    }

    void MakeValid();

    vec3 Corner(int index) const;

    void Expand(const vec3& point);

    void Union(const AABB& o);

    void Intersection(const AABB& o);

    void ApplyMatrix(const mat4& mat4);

    void FromCenterSize(const vec3& center, const vec3& size);

    inline vec3 Center() const { return 0.5f * (mMin + mMax); }
    inline vec3 Size() const { return mMax - mMin; }

    const vec3& GetMin() const { return mMin; }
    const vec3& GetMax() const { return mMax; }

    inline bool Intersects(Ray& ray) const { return TestIntersection::RayAABB(*this, ray); }

private:
    vec3 mMin, mMax;

    friend class TestIntersection;
};
