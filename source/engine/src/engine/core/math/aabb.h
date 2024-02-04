#pragma once
#include "box.h"
#include "intersection.h"

namespace vct {

class AABB : public Box3 {
public:
    using Box3::Box;

    vec3 corner(int index) const;

    void apply_matrix(const mat4& mat);

    void from_center_size(const vec3& center, const vec3& size);

    bool intersects(Ray& ray) const { return TestIntersection::ray_aabb(*this, ray); }

    friend class TestIntersection;
};

}  // namespace vct
