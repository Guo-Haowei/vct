#include "Ray.h"

Ray Ray::Inverse(const mat4& inverseMatrix) const
{
    vec3 inversedStart = vec3(inverseMatrix * vec4(mStart, 1.0f));
    vec3 inversedEnd = vec3(inverseMatrix * vec4(mEnd, 1.0f));
    Ray inversedRay(inversedStart, inversedEnd);
    inversedRay.mDist = mDist;
    return inversedRay;
}
