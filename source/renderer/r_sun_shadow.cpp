#include "r_sun_shadow.h"

#include "common/com_dvars.h"
#include "common/com_system.h"
#include "common/main_window.h"
#include "universal/core_assert.h"
#include "universal/dvar_api.h"
#include "universal/print.h"

#pragma optimize( "", off )

static void printPoint( const vec3& v, bool isMin )
{
    Com_Printf( "%s{%f %f %f}", isMin ? "min" : "max", v.x, v.y, v.z );
}

static void printBox( const AABB& box )
{
    printPoint( box.min, true );
    printPoint( box.max, false );
}

static mat4 LightSpaceMatrix( const AABB& aabb, vec3 orientation )
{
    const vec3 C       = aabb.Center();
    const vec3 extents = aabb.Size();
    const float size   = 0.5f * glm::max( extents.x, glm::max( extents.y, extents.z ) );

    // TODO: configure up
    const mat4 V = glm::lookAt( C + orientation * size, C, vec3( 0, 1, 0 ) );
    const mat4 P = glm::ortho( -size, size, -size, size, 0.0f, 2.0f * size );
    return P * V;
}

void LightSpaceMatrix( const Camera& camera, vec3 lightDir, mat4 lightPVs[NUM_CASCADES] )
{
    const vec4 cascades     = Dvar_GetVec4( cam_cascades );
    const mat4 vInv         = glm::inverse( camera.view() );  // inversed V
    const float tanHalfHFOV = glm::tan( camera.fovy * 0.5f );
    const float tanHalfVFOV = glm::tan( camera.fovy * camera.aspect * 0.5f );

    for ( int idx = 0; idx < NUM_CASCADES; ++idx )
    {
        float xn = cascades[idx] * tanHalfHFOV;
        float xf = cascades[idx + 1] * tanHalfHFOV;
        float yn = cascades[idx] * tanHalfVFOV;
        float yf = cascades[idx + 1] * tanHalfVFOV;

        const vec4 points[] = {
            // near face
            vec4( xn, yn, cascades[idx], 1.0 ),
            vec4( -xn, yn, cascades[idx], 1.0 ),
            vec4( xn, -yn, cascades[idx], 1.0 ),
            vec4( -xn, -yn, cascades[idx], 1.0 ),

            // far face
            vec4( xf, yf, cascades[idx + 1], 1.0 ),
            vec4( -xf, yf, cascades[idx + 1], 1.0 ),
            vec4( xf, -yf, cascades[idx + 1], 1.0 ),
            vec4( -xf, -yf, cascades[idx + 1], 1.0 )
        };

        AABB box;
        for ( size_t j = 0; j < array_length( points ); ++j )
        {
            const vec3 p = vec3( ( vInv * points[j] ) );
            box.Expand( p );
        }

        lightPVs[idx] = LightSpaceMatrix( box, lightDir );
    }
}
