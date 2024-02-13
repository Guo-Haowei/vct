#pragma once
#include "geomath.h"

namespace vct {

class Degree {
public:
    explicit constexpr Degree() : m_value(0) {}
    explicit constexpr Degree(float degree) : m_value(degree) {}

    Degree& operator+=(Degree d) {
        m_value += d.m_value;
        return *this;
    }
    Degree& operator-=(Degree d) {
        m_value -= d.m_value;
        return *this;
    }
    void clamp(float a, float b) { m_value = glm::clamp(m_value, a, b); }
    float to_rad() const { return glm::radians(m_value); }
    float sin() const { return glm::sin(to_rad()); }
    float cos() const { return glm::cos(to_rad()); }

private:
    float m_value;
};

}  // namespace vct
