#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <limits>

using glm::vec3;

constexpr float INFINITY_F = std::numeric_limits<float>::infinity();

struct Box3D
{
    vec3 min = vec3(INFINITY_F);
    vec3 max = vec3(-INFINITY_F);

    void expand(const vec3& point)
    {
        max.x = glm::max(point.x, max.x);
        max.y = glm::max(point.y, max.y);
        max.z = glm::max(point.z, max.z);
        min.x = glm::min(point.x, min.x);
        min.y = glm::min(point.y, min.y);
        min.z = glm::min(point.z, min.z);
    }

    void expand(const Box3D& o)
    {
        max.x = glm::max(o.max.x, max.x);
        max.y = glm::max(o.max.y, max.y);
        max.z = glm::max(o.max.z, max.z);
        min.x = glm::min(o.min.x, min.x);
        min.y = glm::min(o.min.y, min.y);
        min.z = glm::min(o.min.z, min.z);
    }

};

static std::ostream& operator<<(std::ostream& os, const Box3D& b)
{
    os << "{ min: " << b.min.x << ", " << b.min.y << ", " << b.min.x << ";";
    os << " max: " << b.max.x << ", " << b.max.y << ", " << b.max.y << " }";
    return os;
}

