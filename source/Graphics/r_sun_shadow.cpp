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

    glViewport( 0 * res, 0, res, res );

    Frame dummy;
    g_gfxMgr->DrawBatch( dummy );

    g_shadowRT.Unbind();
    glCullFace( GL_BACK );
}