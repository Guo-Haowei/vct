#include "GBufferPass.hpp"

// @TODO: remove
#include "Core/WindowManager.h"

void GBufferPass::Draw( Frame& frame )
{
    auto PSO = m_pPipelineStateManager->GetPipelineState( "GBUFFER" );
    m_pGraphicsManager->SetPipelineState( PSO );

    ivec2 extent = g_wndMgr->FrameSize();
    glViewport( 0, 0, extent.x, extent.y );

    m_pGraphicsManager->DrawBatch( frame );
}