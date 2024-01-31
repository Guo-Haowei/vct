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
constexpr inline T min_val(const T& a, const T& b) {
    return glm::min(a, b);
    // return a < b ? a : b;
}

template<typename T>
constexpr inline T max_val(const T& a, const T& b) {
    return glm::max(a, b);
    // return a > b ? a : b;
}

template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr inline T align(T size, T alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

constexpr inline uint32_t next_power_of_two(uint32_t x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

// @TODO: refactor
template<typename T>
T Square(const T& a) {
    return a * a;
}

template<typename T>
inline T Lerp(const T& a, const T& b, const T& f) {
    return a + f * (b - a);
}

static inline void Decompose(const mat4& matrix, vec3& scale, vec4& rotation, vec3& translation) {
    vec3 _skew;
    vec4 _perspective;
    quat quaternion;
    glm::decompose(matrix, scale, quaternion, translation, _skew, _perspective);
    rotation.x = quaternion.x;
    rotation.y = quaternion.y;
    rotation.z = quaternion.z;
    rotation.w = quaternion.w;
}

static inline float SaturateF(float x) { return min_val(1.0f, max_val(0.0f, x)); }
