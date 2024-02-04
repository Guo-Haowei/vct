#include "frustum.h"

#include "aabb.h"

namespace vct {

// https://stackoverflow.com/questions/12836967/extracting-view-frustum-planes-hartmann-gribbs-method
Frustum::Frustum(const mat4& PV) {
    m_left.m_normal.x = PV[0][3] + PV[0][0];
    m_left.m_normal.y = PV[1][3] + PV[1][0];
    m_left.m_normal.z = PV[2][3] + PV[2][0];
    m_left.m_dist = PV[3][3] + PV[3][0];

    m_right.m_normal.x = PV[0][3] - PV[0][0];
    m_right.m_normal.y = PV[1][3] - PV[1][0];
    m_right.m_normal.z = PV[2][3] - PV[2][0];
    m_right.m_dist = PV[3][3] - PV[3][0];

    m_top.m_normal.x = PV[0][3] - PV[0][1];
    m_top.m_normal.y = PV[1][3] - PV[1][1];
    m_top.m_normal.z = PV[2][3] - PV[2][1];
    m_top.m_dist = PV[3][3] - PV[3][1];

    m_bottom.m_normal.x = PV[0][3] + PV[0][1];
    m_bottom.m_normal.y = PV[1][3] + PV[1][1];
    m_bottom.m_normal.z = PV[2][3] + PV[2][1];
    m_bottom.m_dist = PV[3][3] + PV[3][1];

    m_near.m_normal.x = PV[0][3] + PV[0][2];
    m_near.m_normal.y = PV[1][3] + PV[1][2];
    m_near.m_normal.z = PV[2][3] + PV[2][2];
    m_near.m_dist = PV[3][3] + PV[3][2];

    m_far.m_normal.x = PV[0][3] - PV[0][2];
    m_far.m_normal.y = PV[1][3] - PV[1][2];
    m_far.m_normal.z = PV[2][3] - PV[2][2];
    m_far.m_dist = PV[3][3] - PV[3][2];
}

bool Frustum::intersects(const AABB& box) const {
    const vec3& box_min = box.get_min();
    const vec3& box_max = box.get_max();
    for (int i = 0; i < 6; ++i) {
        const Plane& plane = this->operator[](i);
        vec3 p;
        p.x = plane.m_normal.x > static_cast<float>(0) ? box_max.x : box_min.x;
        p.y = plane.m_normal.y > static_cast<float>(0) ? box_max.y : box_min.y;
        p.z = plane.m_normal.z > static_cast<float>(0) ? box_max.z : box_min.z;

        if (plane.distance(p) < static_cast<float>(0)) {
            return false;
        }
    }

    return true;
}

}  // namespace vct
