#include "intersection.h"

#include "aabb.h"
#include "ray.h"

namespace vct {

bool TestIntersection::ray_aabb(const AABB& aabb, Ray& ray) {
    const vec3 direction = ray.m_end - ray.m_start;

    vec3 inv_d = vec3(1) / direction;
    vec3 t0s = (aabb.m_min - ray.m_start) * inv_d;
    vec3 t1s = (aabb.m_max - ray.m_start) * inv_d;

    vec3 tsmaller = glm::min(t0s, t1s);
    vec3 tbigger = glm::max(t0s, t1s);

    float tmin = glm::max(-FLT_MAX, glm::max(tsmaller.x, glm::max(tsmaller.y, tsmaller.z)));
    float tmax = glm::min(FLT_MAX, glm::min(tbigger.x, glm::min(tbigger.y, tbigger.z)));

    // check bounding box
    if (tmin >= tmax || tmin <= 0.0f || tmin >= ray.m_dist) {
        return false;
    }

    ray.m_dist = tmin;
    return true;
}

bool TestIntersection::ray_triangle(const vec3& a, const vec3& b, const vec3& c, Ray& ray) {
    // P = A + u(B - A) + v(C - A) => O - A = -tD + u(B - A) + v(C - A)
    // -tD + uAB + vAC = AO
    const vec3 direction = ray.m_end - ray.m_start;
    const vec3 ab = b - a;
    const vec3 ac = c - a;
    const vec3 P = glm::cross(direction, ac);
    const float det = glm::dot(ab, P);
    if (det < glm::epsilon<float>()) {
        return false;
    }

    const float inv_det = 1.0f / det;
    const vec3 AO = ray.m_start - a;

    const vec3 q = glm::cross(AO, ab);
    const float u = glm::dot(AO, P) * inv_det;
    const float v = glm::dot(direction, q) * inv_det;

    if (u < 0.0 || v < 0.0 || u + v > 1.0) {
        return false;
    }

    const float t = dot(ac, q) * inv_det;
    if (t < glm::epsilon<float>() || t >= ray.m_dist) {
        return false;
    }

    ray.m_dist = t;
    return true;
}

}  // namespace vct
