#include "r_sun_shadow.h"

#include <unordered_map>

#include "RenderTarget.h"
#include "common/com_dvars.h"
#include "common/com_misc.h"
#include "common/main_window.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "r_shader.h"
#include "scene/Scene.h"
#include "universal/core_assert.h"
#include "universal/dvar_api.h"
#include "universal/print.h"

#pragma optimize( "", off )

#ifdef max
#undef max
#endif

extern std::unordered_map<const MeshComponent*, MeshData> g_meshLUT;
extern std::unordered_map<const Material*, MaterialData> g_matLUT;

extern DepthRenderTarget g_shadowBuffer;

void R_LightSpaceMatrix( const Camera& camera, vec3 lightDir, mat4 lightPVs[NUM_CASCADES] )
{
    const vec4 cascades     = Dvar_GetVec4( cam_cascades );
    const mat4 vInv         = glm::inverse( camera.View() );  // inversed V
    const float tanHalfHFOV = glm::tan( 0.5f * camera.fovy );
    const float tanHalfVFOV = glm::tan( 0.5f * camera.fovy * camera.GetAspect() );

    for ( int idx = 0; idx < NUM_CASCADES; ++idx )
    {
        constexpr float offset = 0.1f;
        const float xn         = cascades[0] * tanHalfHFOV;
        const float xf         = cascades[idx + 1] * tanHalfHFOV;
        const float yn         = cascades[0] * tanHalfVFOV;
        const float yf         = cascades[idx + 1] * tanHalfVFOV;
        const float zNear      = cascades[0];
        const float zFar       = cascades[idx + 1];

        const vec3 closest = vInv * vec4( -xf, -yf, zNear, 1.0f );
        const vec3 farest  = vInv * vec4( +xf, +yf, zFar, 1.0f );
        const vec3 center  = 0.5f * ( farest + closest );
        const float size   = ( 0.5f + offset ) * glm::distance( closest, farest );
        const mat4 V       = glm::lookAt( center + glm::normalize( lightDir ) * size, center, vec3( 0, 1, 0 ) );
        const mat4 P       = glm::ortho( -size, size, -size, size, 0.0f, 2.0f * size );
        lightPVs[idx]      = P * V;
    }
}

void R_ShadowPass()
{
    const Scene& scene = Com_GetScene();
    g_shadowBuffer.bind();

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_FRONT );
    glClear( GL_DEPTH_BUFFER_BIT );
    const GlslProgram& program = R_GetShaderProgram( ProgramType::SHADOW );
    program.Use();
    static GLint PVMLocation = program.GetUniformLocation( "u_PVM" );

    // render scene 3 times
    for ( int idx = 0; idx < NUM_CASCADES; ++idx )
    {
        glViewport( idx * SHADOW_MAP_RESOLUTION, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION );
        const mat4& PV = g_perframeCache.cache.LightPVs[idx];
        const Frustum frustum( PV );
        for ( const GeometryNode& node : scene.geometryNodes )
        {
            const mat4 PVM = PV * node.transform;
            program.SetUniform( PVMLocation, PVM );
            for ( const Geometry& geom : node.geometries )
            {
                if ( !geom.visible )
                {
                    continue;
                }
                if ( !frustum.Intersect( geom.boundingBox ) )
                {
                    continue;
                }

                const auto& meshPair = g_meshLUT.find( geom.pMesh );
                core_assert( meshPair != g_meshLUT.end() );
                const MeshData& drawData = meshPair->second;

                glBindVertexArray( drawData.vao );
                glDrawElements( GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0 );
            }
        }
    }

    g_shadowBuffer.unbind();
    glCullFace( GL_BACK );
}