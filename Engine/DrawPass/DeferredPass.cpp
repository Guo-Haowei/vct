#include "DeferredPass.hpp"

#include "Interface/IApplication.hpp"

// @TODO: remove
extern void R_DrawQuad();

void DeferredPass::Draw( Frame& frame )
{
    unused( frame );

    auto PSO = m_pPipelineStateManager->GetPipelineState( "VCT" );
    m_pGraphicsManager->SetPipelineState( PSO );

    auto pApp = m_pPipelineStateManager->GetAppPointer();
    int width = 0, height = 0;
    pApp->GetFramebufferSize( width, height );
    glViewport( 0, 0, width, height );

    R_DrawQuad();
}