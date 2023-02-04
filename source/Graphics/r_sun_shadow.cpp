#include "r_sun_shadow.h"

#include "GraphicsManager.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Core/com_dvars.h"
#include "Core/com_misc.h"
#include "Core/WindowManager.h"
#include "Core/scene.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "r_rendertarget.h"
#include "universal/dvar_api.h"

#include "PipelineStateManager.hpp"

#ifdef max
#undef max
#endif

mat4 R_HackLightSpaceMatrix( const vec3& lightDir )
{
    const Scene& scene = Com_GetScene();
    const vec3 center = scene.boundingBox.Center();
    const vec3 extents = scene.boundingBox.Size();
    const float size = 0.5f * glm::max( extents.x, glm::max( extents.y, extents.z ) );
    const mat4 V = glm::lookAt( center + glm::normalize( lightDir ) * size, center, vec3( 0, 1, 0 ) );
    const mat4 P = glm::ortho( -size, size, -size, size, 0.0f, 2.0f * size );
    return P * V;
}

void R_ShadowPass()
{
    auto PSO = g_pPipelineStateManager->GetPipelineState( "SHADOW" );
    g_gfxMgr->SetPipelineState( PSO );

    const Scene& scene = Com_GetScene();
    g_shadowRT.Bind();

    glCullFace( GL_FRONT );
    glClear( GL_DEPTH_BUFFER_BIT );

    const int res = Dvar_GetInt( r_shadowRes );
    // render scene 3 times

    const mat4 oldPV = g_perFrameCache.cache.PV;

    glViewport( 0 * res, 0, res, res );
    const mat4& PV = g_perFrameCache.cache.LightPV;
    const Frustum frustum( PV );
    g_perFrameCache.cache.PV = PV;
    g_perFrameCache.Update();

    Frame dummy;
    g_gfxMgr->DrawBatch( dummy );

    // restore
    g_perFrameCache.cache.PV = oldPV;
    g_perFrameCache.Update();

    g_shadowRT.Unbind();
    glCullFace( GL_BACK );
}