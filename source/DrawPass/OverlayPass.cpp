#include "OverlayPass.hpp"

// @TODO: remove
#include "Core/com_dvars.h"
#include "Core/WindowManager.h"
#include "Graphics/gl_utils.h"

void OverlayPass::Draw( Frame& frame )
{
    auto PSO = m_pPipelineStateManager->GetPipelineState( "OVERLAY" );
    m_pGraphicsManager->SetPipelineState( PSO );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    ivec2 extent = g_wndMgr->FrameSize();
    glViewport( 0, 0, extent.x, extent.y );
    glBindVertexArray( g_quad.vao );

    const int instances = Dvar_GetBool( r_showDebugTexture ) ? NUM_OVERLAYS : 1;
    glDrawArraysInstanced( GL_TRIANGLES, 0, 6, instances );
}