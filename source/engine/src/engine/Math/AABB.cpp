#include "AABB.h"

#include "Core/Check.h"

using namespace vct;

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
vec3 AABB::Corner(int index) const {
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

void AABB::ApplyMatrix(const mat4& mat4) {
    vec4 points[] = { vec4(mMin.x, mMin.y, mMin.z, 1.0f), vec4(mMin.x, mMin.y, mMax.z, 1.0f),
                      vec4(mMin.x, mMax.y, mMin.z, 1.0f), vec4(mMin.x, mMax.y, mMax.z, 1.0f),
                      vec4(mMax.x, mMin.y, mMin.z, 1.0f), vec4(mMax.x, mMin.y, mMax.z, 1.0f),
                      vec4(mMax.x, mMax.y, mMin.z, 1.0f), vec4(mMax.x, mMax.y, mMax.z, 1.0f) };
    static_assert(array_length(points) == 8);

    AABB newBox;
    for (size_t i = 0; i < array_length(points); ++i) {
        newBox.Expand(vec3(mat4 * points[i]));
    }

    mMin = newBox.mMin;
    mMax = newBox.mMax;
}

void AABB::FromCenterSize(const vec3& center, const vec3& size) {
    vec3 half = 0.5f * size;
    mMin = center - half;
    mMax = center + half;
}
