#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <limits>

using glm::vec3;
using glm::mat4;

namespace glm {

constexpr float INFINITY_F = std::numeric_limits<float>::infinity();
constexpr float QUARTER_PI_F = 0.25f * glm::pi<float>();
constexpr float HALF_PI_F = 0.5f * glm::pi<float>();
constexpr float PI_F = glm::pi<float>();
constexpr float TWO_PI_F = 2.0f * glm::pi<float>();

const vec3 VEC3_ZERO        { 0.0f, 0.0f, 0.0f };
const vec3 VEC3_UNIT_SCALE  { 1.0f, 1.0f, 1.0f };
const vec3 VEC3_UNIT_X      { 1.0f, 0.0f, 0.0f };
const vec3 VEC3_UNIT_Y      { 0.0f, 1.0f, 0.0f };
const vec3 VEC3_UNIT_Z      { 0.0f, 0.0f, 1.0f };

const vec4 VEC4_ZERO        { 0.0f, 0.0f, 0.0f, 0.0f };
const vec4 VEC4_UNIT_SCALE  { 1.0f, 1.0f, 1.0f, 0.0f };
const vec4 VEC4_UNIT_X      { 1.0f, 0.0f, 0.0f, 0.0f };
const vec4 VEC4_UNIT_Y      { 0.0f, 1.0f, 0.0f, 0.0f };
const vec4 VEC4_UNIT_Z      { 0.0f, 0.0f, 1.0f, 0.0f };
const vec4 VEC4_UNIT_W      { 0.0f, 0.0f, 0.0f, 1.0f };
const mat4 MAT4_IDENTITY    { { 1.0f, 0.0f, 0.0f, 0.0f },
                              { 0.0f, 1.0f, 0.0f, 0.0f },
                              { 0.0f, 0.0f, 1.0f, 0.0f },
                              { 0.0f, 0.0f, 0.0f, 1.0f } };

} // namespace glm

using namespace glm;

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
    os << "{ \"min\" : [ " << b.min.x << ", " << b.min.y << ", " << b.min.x << "],";
    os << "\"max\" : [ " << b.max.x << ", " << b.max.y << ", " << b.max.y << " ] }";
    return os;
}

