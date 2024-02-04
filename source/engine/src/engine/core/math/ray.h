#pragma once
#include "intersection.h"

namespace vct {

class Ray {
public:
    Ray(const vec3& start, const vec3& end) : m_start(start), m_end(end), m_dist(1.0f) {}

    Ray inverse(const mat4& inverseMatrix) const;

    vec3 direction() const { return glm::normalize(m_end - m_start); }

    bool intersects(const AABB& aabb) { return TestIntersection::ray_aabb(aabb, *this); }
    bool intersects(const vec3& A, const vec3& B, const vec3& C) { return TestIntersection::ray_triangle(A, B, C, *this); }

    // Used for inverse ray intersection update result
    void copy_dist(const Ray& other) { m_dist = other.m_dist; }

private:
    const vec3 m_start;
    const vec3 m_end;
    float m_dist;  // if dist is in range of [0, 1.0f), it hits something

    friend class TestIntersection;
};

}  // namespace vct
