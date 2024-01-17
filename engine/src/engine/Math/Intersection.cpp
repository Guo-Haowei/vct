#include "Intersection.h"
#include "AABB.h"
#include "Ray.h"

bool TestIntersection::RayAABB(const AABB& aabb, Ray& ray)
{
    const vec3 direction = ray.mEnd - ray.mStart;

    vec3 invD = vec3(1) / direction;
    vec3 t0s = (aabb.mMin - ray.mStart) * invD;
    vec3 t1s = (aabb.mMax - ray.mStart) * invD;

    vec3 tsmaller = glm::min(t0s, t1s);
    vec3 tbigger = glm::max(t0s, t1s);

    float tmin = glm::max(-FLT_MAX, glm::max(tsmaller.x, glm::max(tsmaller.y, tsmaller.z)));
    float tmax = glm::min(FLT_MAX, glm::min(tbigger.x, glm::min(tbigger.y, tbigger.z)));

    // check bounding box
    if (tmin >= tmax || tmin <= 0.0f || tmin >= ray.mDist)
    {
        return false;
    }

    ray.mDist = tmin;
    return true;
}

bool TestIntersection::RayTriangle(const vec3& A, const vec3& B, const vec3& C, Ray& ray)
{
    // P = A + u(B - A) + v(C - A) => O - A = -tD + u(B - A) + v(C - A)
    // -tD + uAB + vAC = AO
    const vec3 direction = ray.mEnd - ray.mStart;
    const vec3 AB = B - A;
    const vec3 AC = C - A;
    const vec3 P = glm::cross(direction, AC);
    const float det = glm::dot(AB, P);
    if (det < glm::epsilon<float>())
    {
        return false;
    }

    const float invDet = 1.0f / det;
    const vec3 AO = ray.mStart - A;

    const vec3 Q = glm::cross(AO, AB);
    const float u = glm::dot(AO, P) * invDet;
    const float v = glm::dot(direction, Q) * invDet;

    if (u < 0.0 || v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    const float t = dot(AC, Q) * invDet;
    if (t < glm::epsilon<float>() || t >= ray.mDist)
    {
        return false;
    }

    ray.mDist = t;
    return true;
}