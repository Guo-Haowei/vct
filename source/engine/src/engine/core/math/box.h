#pragma once
#include "core/math/geomath.h"

namespace vct {

template<size_t N>
class Box {
    using vec_type = glm::vec<N, float, glm::defaultp>;
    using self_type = Box<N>;

public:
    Box() { make_invalid(); }

    Box(const vec_type& min, const vec_type& max) : m_min(min), m_max(max) {}

    void make_invalid() {
        m_min = vec_type(std::numeric_limits<float>::infinity());
        m_max = vec_type(-std::numeric_limits<float>::infinity());
    }

    void make_valid() {
        const vec_type size = glm::abs(m_max - m_min);
        constexpr float delta = 0.0001f;
        if (size.x == 0.0f) {
            m_min.x -= delta;
            m_max.x += delta;
        }
        if (size.y == 0.0f) {
            m_min.y -= delta;
            m_max.y += delta;
        }
        if constexpr (N > 2) {
            if (size.z == 0.0f) {
                m_min.z -= delta;
                m_max.z += delta;
            }
        }
    }

    void expand_point(const vec_type& point) {
        m_min = glm::min(m_min, point);
        m_max = glm::max(m_max, point);
    }

    void union_box(const self_type& o) {
        m_min = glm::min(m_min, o.m_min);
        m_max = glm::max(m_max, o.m_max);
    }

    void intersect_box(const self_type& o) {
        m_min = glm::max(m_min, o.m_min);
        m_max = glm::min(m_max, o.m_max);
    }

    vec_type center() const { return 0.5f * (m_min + m_max); }
    vec_type size() const { return m_max - m_min; }

    const vec_type& get_min() const { return m_min; }
    const vec_type& get_max() const { return m_max; }

protected:
    vec_type m_min;
    vec_type m_max;
};

using Box2 = Box<2>;
using Box3 = Box<3>;

}  // namespace vct
