#pragma once
#include "Intersection.h"
#include "Box.h"

class AABB : public Box3
{
public:
    using Box3::Box;

    vec3 Corner(int index) const;

    void ApplyMatrix(const mat4& mat4);

    void FromCenterSize(const vec3& center, const vec3& size);

    inline bool Intersects(Ray& ray) const { return TestIntersection::RayAABB(*this, ray); }

    friend class TestIntersection;
};
