#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/vector_angle.hpp"
#ifdef assert
#undef assert
#endif

#ifdef near
#undef near
#endif  // #ifdef near
#ifdef far
#undef far
#endif  // #ifdef far
#ifdef min
#undef min
#endif  // #ifdef min
#ifdef max
#undef max
#endif  // #ifdef max

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

//------------------------------------------------------------------------------
// AABB
//------------------------------------------------------------------------------
struct AABB {
    vec3 min, max;

    constexpr AABB::AABB()
        : min( vec3( std::numeric_limits<float>::infinity() ) )
        , max( vec3( -std::numeric_limits<float>::infinity() ) )
    {
    }
    constexpr AABB( const vec3& min, const vec3& max );

    bool Valid() const;
    void Expand( const vec3& point );

    void Expand( const vec3* points, size_t count );

    void Intersection( const AABB& o );
    void Union( const AABB& o );

    void ApplyMatrix( const mat4& mat4 );

    void FromCenterSize( const vec3& center, const vec3& size )
    {
        vec3 half = 0.5f * size;
        min       = center - half;
        max       = center + half;
    }

    vec3 Center() const
    {
        return 0.5f * ( min + max );
    }

    vec3 Size() const
    {
        return max - min;
    }
};

//------------------------------------------------------------------------------
// Plane3
//------------------------------------------------------------------------------
struct Plane3 {
    vec3 n;
    float d;

    Plane3() = default;
    Plane3( const vec3& n, float d );
    inline float Distance( const vec3& p ) const { return glm::dot( p, n ) + d; }
};

//------------------------------------------------------------------------------
// Frustum
//------------------------------------------------------------------------------
struct Frustum {
    Plane3 left;
    Plane3 right;
    Plane3 top;
    Plane3 bottom;
    Plane3 near;
    Plane3 far;

    Frustum() = default;
    Frustum( const mat4& PV );

    Plane3& operator[]( int i )
    {
        return reinterpret_cast<Plane3*>( this )[i];
    }

    const Plane3& operator[]( int i ) const
    {
        return reinterpret_cast<const Plane3*>( this )[i];
    }

    bool Intersect( const AABB& box ) const;
};

//------------------------------------------------------------------------------
// Ray
//------------------------------------------------------------------------------

struct Ray {
    static constexpr float kRayMin = glm::epsilon<float>();
    static constexpr float kRayMax = std::numeric_limits<float>::infinity();

    vec3 origin_;
    vec3 direction_;
    float distance_ = kRayMax;

    bool Intersects( vec3 A, vec3 B, vec3 C );
    bool Intersects( const AABB& box );
};

// TODO: refactor
// void BoxWireframe( vec3* outPoints, uvec2* outIndices, float size = 0.5f );

// void Box( std::vector<vec3>& outPoints,
//           std::vector<uint32_t>& outIndices,
//           float size = 0.5f );

// void BoxWithNormal( std::vector<vec3>& outPoints,
//                     std::vector<vec3>& outNomrals,
//                     std::vector<uint32_t>& outIndices,
//                     float size = 0.5f );
