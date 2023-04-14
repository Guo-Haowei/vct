#include "AABB.h"
#include "Core/Check.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

void AABB::MakeInvalid()
{
    mMin = vec3(std::numeric_limits<float>::infinity());
    mMax = vec3(-std::numeric_limits<float>::infinity());
}

AABB::AABB(const vec3& min, const vec3& max)
    : mMin(min), mMax(max)
{
    check(IsValid());
}

void AABB::MakeValid()
{
    const vec3 size = mMax - mMin;
    constexpr float delta = 0.0001f;
    if (size.x == 0.0f)
    {
        mMin.x -= delta;
        mMax.x += delta;
    }
    if (size.y == 0.0f)
    {
        mMin.y -= delta;
        mMax.y += delta;
    }
    if (size.z == 0.0f)
    {
        mMin.z -= delta;
        mMax.z += delta;
    }
}

#if 0
 *        E__________________ H
 *       /|                 /|
 *      / |                / |
 *     /  |               /  |
 *   A/___|______________/D  |
 *    |   |              |   |
 *    |   |              |   |
 *    |   |              |   |
 *    |  F|______________|___|G
 *    |  /               |  /
 *    | /                | /
 *   B|/_________________|C
 *
 * A, B, B, C, C, D, D, A, E, F, F, G, G, H, H, E, A, E, B, F, D, H, C, G
#endif
vec3 AABB::Corner(int index) const
{
    // clang-format off
    switch (index)
    {
        case 0: return vec3(mMin.x, mMax.y, mMax.z); // A
        case 1: return vec3(mMin.x, mMin.y, mMax.z); // B
        case 2: return vec3(mMax.x, mMin.y, mMax.z); // C
        case 3: return vec3(mMax.x, mMax.y, mMax.z); // D
        case 4: return vec3(mMin.x, mMax.y, mMin.z); // E
        case 5: return vec3(mMin.x, mMin.y, mMin.z); // F
        case 6: return vec3(mMax.x, mMin.y, mMin.z); // G
        case 7: return vec3(mMax.x, mMax.y, mMin.z); // H
    }
    // clang-format on
    check(0);
    return vec3(0);
}

void AABB::Expand(const vec3& point)
{
    mMin = glm::min(mMin, point);
    mMax = glm::max(mMax, point);
}

void AABB::Union(const AABB& o)
{
    mMin = glm::min(mMin, o.mMin);
    mMax = glm::max(mMax, o.mMax);
}

void AABB::Intersection(const AABB& o)
{
    mMin = glm::max(mMin, o.mMin);
    mMax = glm::min(mMax, o.mMax);
}

void AABB::ApplyMatrix(const mat4& mat4)
{
    vec4 points[] = {
        vec4(mMin.x, mMin.y, mMin.z, 1.0f),
        vec4(mMin.x, mMin.y, mMax.z, 1.0f),
        vec4(mMin.x, mMax.y, mMin.z, 1.0f),
        vec4(mMin.x, mMax.y, mMax.z, 1.0f),
        vec4(mMax.x, mMin.y, mMin.z, 1.0f),
        vec4(mMax.x, mMin.y, mMax.z, 1.0f),
        vec4(mMax.x, mMax.y, mMin.z, 1.0f),
        vec4(mMax.x, mMax.y, mMax.z, 1.0f)
    };
    static_assert(array_length(points) == 8);

    AABB newBox;
    for (size_t i = 0; i < array_length(points); ++i)
    {
        newBox.Expand(vec3(mat4 * points[i]));
    }

    mMin = newBox.mMin;
    mMax = newBox.mMax;
}

void AABB::FromCenterSize(const vec3& center, const vec3& size)
{
    vec3 half = 0.5f * size;
    mMin = center - half;
    mMax = center + half;
}
