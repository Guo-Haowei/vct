#pragma once
#include "GeoMath.h"

template<size_t N>
class Box {
    using Vec = glm::vec<N, float, glm::defaultp>;
    using Self = Box<N>;

public:
    Box() { MakeInvalid(); }

    Box(const Vec& min, const Vec& max) : mMin(min), mMax(max) {}

    void MakeInvalid() {
        mMin = Vec(std::numeric_limits<float>::infinity());
        mMax = Vec(-std::numeric_limits<float>::infinity());
    }

    void MakeValid() {
        const Vec size = glm::abs(mMax - mMin);
        constexpr float delta = 0.0001f;
        if (size.x == 0.0f) {
            mMin.x -= delta;
            mMax.x += delta;
        }
        if (size.y == 0.0f) {
            mMin.y -= delta;
            mMax.y += delta;
        }
        if constexpr (N > 2) {
            if (size.z == 0.0f) {
                mMin.z -= delta;
                mMax.z += delta;
            }
        }
    }

    void Expand(const Vec& point) {
        mMin = glm::min(mMin, point);
        mMax = glm::max(mMax, point);
    }

    void Union(const Self& o) {
        mMin = glm::min(mMin, o.mMin);
        mMax = glm::max(mMax, o.mMax);
    }

    void Intersection(const Self& o) {
        mMin = glm::max(mMin, o.mMin);
        mMax = glm::min(mMax, o.mMax);
    }

    Vec Center() const { return 0.5f * (mMin + mMax); }
    Vec Size() const { return mMax - mMin; }

    const Vec& GetMin() const { return mMin; }
    const Vec& GetMax() const { return mMax; }

protected:
    Vec mMin;
    Vec mMax;
};

using Box2 = Box<2>;
using Box3 = Box<3>;
