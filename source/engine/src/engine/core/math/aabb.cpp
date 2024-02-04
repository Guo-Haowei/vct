#include "aabb.h"

namespace vct {

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
vec3 AABB::corner(int index) const {
    // clang-format off
    switch (index)
    {
        case 0: return vec3(m_min.x, m_max.y, m_max.z); // A
        case 1: return vec3(m_min.x, m_min.y, m_max.z); // B
        case 2: return vec3(m_max.x, m_min.y, m_max.z); // C
        case 3: return vec3(m_max.x, m_max.y, m_max.z); // D
        case 4: return vec3(m_min.x, m_max.y, m_min.z); // E
        case 5: return vec3(m_min.x, m_min.y, m_min.z); // F
        case 6: return vec3(m_max.x, m_min.y, m_min.z); // G
        case 7: return vec3(m_max.x, m_max.y, m_min.z); // H
    }
    // clang-format on
    DEV_ASSERT(0);
    return vec3(0);
}

void AABB::apply_matrix(const mat4& mat4) {
    vec4 points[] = { vec4(m_min.x, m_min.y, m_min.z, 1.0f), vec4(m_min.x, m_min.y, m_max.z, 1.0f),
                      vec4(m_min.x, m_max.y, m_min.z, 1.0f), vec4(m_min.x, m_max.y, m_max.z, 1.0f),
                      vec4(m_max.x, m_min.y, m_min.z, 1.0f), vec4(m_max.x, m_min.y, m_max.z, 1.0f),
                      vec4(m_max.x, m_max.y, m_min.z, 1.0f), vec4(m_max.x, m_max.y, m_max.z, 1.0f) };
    static_assert(array_length(points) == 8);

    AABB new_box;
    for (size_t i = 0; i < array_length(points); ++i) {
        new_box.expand_point(vec3(mat4 * points[i]));
    }

    m_min = new_box.m_min;
    m_max = new_box.m_max;
}

void AABB::from_center_size(const vec3& center, const vec3& size) {
    vec3 half = 0.5f * size;
    m_min = center - half;
    m_max = center + half;
}

}  // namespace vct
