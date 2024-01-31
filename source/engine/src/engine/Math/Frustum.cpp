#include "Frustum.h"

#include "AABB.h"

// https://stackoverflow.com/questions/12836967/extracting-view-frustum-planes-hartmann-gribbs-method
Frustum::Frustum(const mat4& PV) {
    mLeft.mNormal.x = PV[0][3] + PV[0][0];
    mLeft.mNormal.y = PV[1][3] + PV[1][0];
    mLeft.mNormal.z = PV[2][3] + PV[2][0];
    mLeft.mDist = PV[3][3] + PV[3][0];

    mRight.mNormal.x = PV[0][3] - PV[0][0];
    mRight.mNormal.y = PV[1][3] - PV[1][0];
    mRight.mNormal.z = PV[2][3] - PV[2][0];
    mRight.mDist = PV[3][3] - PV[3][0];

    mTop.mNormal.x = PV[0][3] - PV[0][1];
    mTop.mNormal.y = PV[1][3] - PV[1][1];
    mTop.mNormal.z = PV[2][3] - PV[2][1];
    mTop.mDist = PV[3][3] - PV[3][1];

    mBottom.mNormal.x = PV[0][3] + PV[0][1];
    mBottom.mNormal.y = PV[1][3] + PV[1][1];
    mBottom.mNormal.z = PV[2][3] + PV[2][1];
    mBottom.mDist = PV[3][3] + PV[3][1];

    mNear.mNormal.x = PV[0][3] + PV[0][2];
    mNear.mNormal.y = PV[1][3] + PV[1][2];
    mNear.mNormal.z = PV[2][3] + PV[2][2];
    mNear.mDist = PV[3][3] + PV[3][2];

    mFar.mNormal.x = PV[0][3] - PV[0][2];
    mFar.mNormal.y = PV[1][3] - PV[1][2];
    mFar.mNormal.z = PV[2][3] - PV[2][2];
    mFar.mDist = PV[3][3] - PV[3][2];
}

bool Frustum::Intersects(const AABB& box) const {
    const vec3& boxMin = box.GetMin();
    const vec3& boxMax = box.GetMax();
    for (int i = 0; i < 6; ++i) {
        const Plane3& plane = this->operator[](i);
        vec3 p;
        p.x = plane.mNormal.x > static_cast<float>(0) ? boxMax.x : boxMin.x;
        p.y = plane.mNormal.y > static_cast<float>(0) ? boxMax.y : boxMin.y;
        p.z = plane.mNormal.z > static_cast<float>(0) ? boxMax.z : boxMin.z;

        if (plane.Distance(p) < static_cast<float>(0)) {
            return false;
        }
    }

    return true;
}
