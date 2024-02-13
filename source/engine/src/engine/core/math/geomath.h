#pragma once

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/vector_angle.hpp>
#pragma warning(pop)

using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::mat3;
using glm::mat4;

using glm::quat;

template<typename T>
constexpr inline T lerp(const T& a, const T& b, const T& f) {
    return a + f * (b - a);
}

template<typename T>
constexpr inline float saturate(T x) { return glm::min(T(1), glm::max(T(0), x)); }

static inline void decompose(const mat4& matrix, vec3& scale, vec4& rotation, vec3& translation) {
    vec3 _skew;
    vec4 _perspective;
    quat quaternion;
    glm::decompose(matrix, scale, quaternion, translation, _skew, _perspective);
    rotation.x = quaternion.x;
    rotation.y = quaternion.y;
    rotation.z = quaternion.z;
    rotation.w = quaternion.w;
}
