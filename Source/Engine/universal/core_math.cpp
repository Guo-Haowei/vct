#include "core_math.h"

#include "core_assert.h"

//------------------------------------------------------------------------------
// AABB
//------------------------------------------------------------------------------

constexpr AABB::AABB( const vec3& min, const vec3& max )
    : min( min ), max( max )
{
    core_assert( min.x < max.x );
    core_assert( min.y < max.y );
    core_assert( min.z < max.z );
}

bool AABB::Valid() const
{
    bool result = ( min.x < max.x );
    result      = result && ( min.y < max.y );
    result      = result && ( min.z < max.z );
    return result;
}

void AABB::Expand( const vec3& point )
{
    min = glm::min( min, point );
    max = glm::max( max, point );
}

void AABB::Expand( const vec3* points, size_t count )
{
    for ( size_t idx = 0; idx < count; ++idx )
    {
        Expand( points[idx] );
    }
}

void AABB::Intersection( const AABB& o )
{
    min = glm::max( min, o.min );
    max = glm::min( max, o.max );
}

void AABB::Union( const AABB& o )
{
    min = glm::min( min, o.min );
    max = glm::max( max, o.max );
}

void AABB::ApplyMatrix( const mat4& mat4 )
{
    vec4 points[8] = {
        vec4( min.x, min.y, min.z, 1.0f ),
        vec4( min.x, min.y, max.z, 1.0f ),
        vec4( min.x, max.y, min.z, 1.0f ),
        vec4( min.x, max.y, max.z, 1.0f ),
        vec4( max.x, min.y, min.z, 1.0f ),
        vec4( max.x, min.y, max.z, 1.0f ),
        vec4( max.x, max.y, min.z, 1.0f ),
        vec4( max.x, max.y, max.z, 1.0f )
    };

    AABB box;
    for ( int i = 0; i < 8; ++i )
    {
        box.Expand( vec3( mat4 * points[i] ) );
    }

    min = box.min;
    max = box.max;
}

//------------------------------------------------------------------------------
// Plane3
//------------------------------------------------------------------------------

Plane3::Plane3( const vec3& n, float d )
    : n( n ), d( d )
{
}

//------------------------------------------------------------------------------
// Frustum
//------------------------------------------------------------------------------

// https://stackoverflow.com/questions/12836967/extracting-view-frustum-planes-hartmann-gribbs-method
Frustum::Frustum( const mat4& PV )
{
    // Left clipping plane
    left.n.x = PV[0][3] + PV[0][0];
    left.n.y = PV[1][3] + PV[1][0];
    left.n.z = PV[2][3] + PV[2][0];
    left.d   = PV[3][3] + PV[3][0];
    // Right clipping plane
    right.n.x = PV[0][3] - PV[0][0];
    right.n.y = PV[1][3] - PV[1][0];
    right.n.z = PV[2][3] - PV[2][0];
    right.d   = PV[3][3] - PV[3][0];
    // Top clipping plane
    top.n.x = PV[0][3] - PV[0][1];
    top.n.y = PV[1][3] - PV[1][1];
    top.n.z = PV[2][3] - PV[2][1];
    top.d   = PV[3][3] - PV[3][1];
    // Bottom clipping plane
    bottom.n.x = PV[0][3] + PV[0][1];
    bottom.n.y = PV[1][3] + PV[1][1];
    bottom.n.z = PV[2][3] + PV[2][1];
    bottom.d   = PV[3][3] + PV[3][1];
    // Near clipping plane
    near.n.x = PV[0][3] + PV[0][2];
    near.n.y = PV[1][3] + PV[1][2];
    near.n.z = PV[2][3] + PV[2][2];
    near.d   = PV[3][3] + PV[3][2];
    // Far clipping plane
    far.n.x = PV[0][3] - PV[0][2];
    far.n.y = PV[1][3] - PV[1][2];
    far.n.z = PV[2][3] - PV[2][2];
    far.d   = PV[3][3] - PV[3][2];
}

bool Frustum::Intersect( const AABB& box ) const
{
    for ( int i = 0; i < 6; ++i )
    {
        const Plane3& plane = this->operator[]( i );
        vec3 p;
        p.x = plane.n.x > static_cast<float>( 0 ) ? box.max.x : box.min.x;
        p.y = plane.n.y > static_cast<float>( 0 ) ? box.max.y : box.min.y;
        p.z = plane.n.z > static_cast<float>( 0 ) ? box.max.z : box.min.z;

        if ( plane.Distance( p ) < static_cast<float>( 0 ) )
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Ray
//------------------------------------------------------------------------------
bool Ray::Intersects( vec3 A, vec3 B, vec3 C )
{
    // P = A + u(B - A) + v(C - A) => O - A = -tD + u(B - A) + v(C - A)
    // -tD + uAB + vAC = AO
    const vec3 AB   = B - A;
    const vec3 AC   = C - A;
    const vec3 P    = glm::cross( direction_, AC );
    const float det = glm::dot( AB, P );

    if ( det < glm::epsilon<float>() )
    {
        return false;
    }

    const float invDet = 1.0f / det;
    const vec3 AO      = origin_ - A;
    const vec3 Q       = glm::cross( AO, AB );
    const float u      = glm::dot( AO, P ) * invDet;
    const float v      = glm::dot( direction_, Q ) * invDet;

    if ( u < 0.0 || v < 0.0 || u + v > 1.0 )
    {
        return false;
    }

    const float t = dot( AC, Q ) * invDet;
    if ( t >= distance_ || t < glm::epsilon<float>() )
    {
        return false;
    }

    distance_ = t;
    return true;
}

bool Ray::Intersects( const AABB& box )
{
    using glm::max;
    using glm::min;

    vec3 invD = vec3( 1 ) / direction_;
    vec3 t0s  = ( box.min - origin_ ) * invD;
    vec3 t1s  = ( box.max - origin_ ) * invD;

    vec3 tsmaller = min( t0s, t1s );
    vec3 tbigger  = max( t0s, t1s );

    float tmin = max( Ray::kRayMin, max( tsmaller.x, max( tsmaller.y, tsmaller.z ) ) );
    float tmax = min( Ray::kRayMax, min( tbigger.x, min( tbigger.y, tbigger.z ) ) );

    // check bounding box
    if ( tmin >= tmax || distance_ < tmin )
    {
        return false;
    }

    return true;
}
