#pragma once
#include "GeoMath.h"

class AABB;

struct Plane3 {
    vec3 mNormal;
    float mDist;

    Plane3() = default;
    Plane3(const vec3& normal, float dist) : mNormal(normal), mDist(dist) {}

    inline float Distance(const vec3& p) const { return glm::dot(p, mNormal) + mDist; }
};

class Frustum {
public:
    Frustum() = default;
    Frustum(const mat4& PV);

    Plane3& operator[](int i) { return reinterpret_cast<Plane3*>(this)[i]; }

    const Plane3& operator[](int i) const { return reinterpret_cast<const Plane3*>(this)[i]; }

    bool Intersects(const AABB& box) const;

private:
    Plane3 mLeft;
    Plane3 mRight;
    Plane3 mTop;
    Plane3 mBottom;
    Plane3 mNear;
    Plane3 mFar;

    friend class IntersectionTest;
};
