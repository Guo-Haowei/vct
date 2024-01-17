#pragma once
#include "Intersection.h"

class Ray
{
public:
    Ray(const vec3& start, const vec3& end)
        : mStart(start), mEnd(end), mDist(1.0f)
    {
    }

    Ray Inverse(const mat4& inverseMatrix) const;

    vec3 Direction() const { return glm::normalize(mEnd - mStart); }

    inline bool Intersects(const AABB& aabb) { return TestIntersection::RayAABB(aabb, *this); }
    inline bool Intersects(const vec3& A, const vec3& B, const vec3& C) { return TestIntersection::RayTriangle(A, B, C, *this); }

    // Used for inverse ray intersection update result
    void CopyDist(const Ray& other) { mDist = other.mDist; }

private:
    const vec3 mStart;
    const vec3 mEnd;
    float mDist;  // if dist is in range of [0, 1.0f), it hits something

    friend class TestIntersection;
};
