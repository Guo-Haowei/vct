#include "ray.h"

namespace vct {

Ray Ray::inverse(const mat4& inverseMatrix) const {
    vec3 inversed_start = vec3(inverseMatrix * vec4(m_start, 1.0f));
    vec3 inversed_end = vec3(inverseMatrix * vec4(m_end, 1.0f));
    Ray inversed_ray(inversed_start, inversed_end);
    inversed_ray.m_dist = m_dist;
    return inversed_ray;
}

}  // namespace vct
