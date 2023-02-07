#include "OverlayPass.hpp"

// @TODO: remove
#include "Core/com_dvars.h"
#include "Graphics/gl_utils.h"

#include "Interface/IApplication.hpp"

void OverlayPass::Draw( Frame& frame )
{
    unused( frame );

    auto PSO = m_pPipelineStateManager->GetPipelineState( "OVERLAY" );
    m_pGraphicsManager->SetPipelineState( PSO );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    auto pApp = m_pPipelineStateManager->GetAppPointer();
    int width = 0, height = 0;
    pApp->GetFramebufferSize( width, height );
    glViewport( 0, 0, width, height );

    glBindVertexArray( g_quad.vao );
    const int instances = Dvar_GetBool( r_showDebugTexture ) ? NUM_OVERLAYS : 1;
    glDrawArraysInstanced( GL_TRIANGLES, 0, 6, instances );
}