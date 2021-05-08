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
// box_t
//------------------------------------------------------------------------------
template<int N, typename T>
struct box_t {
    using vec_t  = glm::vec<N, T, glm::defaultp>;
    using this_t = box_t<N, T>;

    vec_t min, max;

    constexpr box_t()
        : min{ std::numeric_limits<T>::infinity() }
        , max{ -std::numeric_limits<T>::infinity() }
    {
        static_assert( N == 2 || N == 3 );
    }

    constexpr box_t( const vec_t& min, const vec_t& max )
        : min( min ), max( max )
    {
        static_assert( N == 2 || N == 3 );
        assert( min.x < max.x );
        assert( min.y < max.y );
        if constexpr ( N > 2 )
        {
            assert( min.z < max.z );
        }
    }

    bool Valid() const
    {
        bool result = ( min.x < max.x ) && ( min.y < max.y );
        if constexpr ( N > 2 )
        {
            result = result && ( min.z < max.z );
        }
        return result;
    }

    void Expand( const vec_t& point )
    {
        min = glm::min( min, point );
        max = glm::max( max, point );
    }

    void Expand( const vec_t* points, size_t count )
    {
        for ( size_t idx = 0; idx < count; ++idx )
        {
            Expand( points[idx] );
        }
    }

    void Intersection( const this_t& o )
    {
        min = glm::max( min, o.min );
        max = glm::min( max, o.max );
    }

    void Union( const this_t& o )
    {
        min = glm::min( min, o.min );
        max = glm::max( max, o.max );
    }

    vec_t Center() const
    {
        return static_cast<T>( 0.5 ) * ( min + max );
    }

    vec_t Size() const
    {
        return max - min;
    }

    void ApplyMatrix( const mat4& mat4 )
    {
        static_assert( N == 3 );
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

        this_t newBox;
        for ( int i = 0; i < 8; ++i )
        {
            newBox.Expand( vec3( mat4 * points[i] ) );
        }

        min = newBox.min;
        max = newBox.max;
    }

    void FromCenterSize( const vec_t& center, const vec_t& size )
    {
        vec_t half = static_cast<T>( 0.5 ) * size;
        min        = center - half;
        max        = center + half;
    }
};

using Box2 = box_t<2, float>;
using Box3 = box_t<3, float>;

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

    bool Intersect( const Box3& box ) const;
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

    bool IntersectsTriangle( vec3 A, vec3 B, vec3 C );
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
