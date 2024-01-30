#pragma once
#include "GeoMath.h"

class Ray;
class AABB;

class TestIntersection {
public:
    static bool RayAABB(const AABB& aabb, Ray& ray);
    static bool RayTriangle(const vec3& A, const vec3& B, const vec3& C, Ray& ray);
};