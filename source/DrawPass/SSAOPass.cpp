#include "SSAOPass.hpp"

// @TODO: remove
#include "Core/WindowManager.h"
extern void R_DrawQuad();

void SSAOPass::Draw( Frame& frame )
{
    auto PSO = m_pPipelineStateManager->GetPipelineState( "SSAO" );
    m_pGraphicsManager->SetPipelineState( PSO );

    ivec2 extent = g_wndMgr->FrameSize();
    glViewport( 0, 0, extent.x, extent.y );

    R_DrawQuad();
}