#include "ForwardPass.hpp"

#include "Manager/BaseApplication.hpp"

void ForwardPass::Draw( Frame& frame )
{
    auto PSO = m_pPipelineStateManager->GetPipelineState( "FORWARD" );
    m_pGraphicsManager->SetPipelineState( PSO );

    auto pApp = m_pPipelineStateManager->GetAppPointer();
    int width = 0, height = 0;
    pApp->GetFramebufferSize( width, height );
    glViewport( 0, 0, width, height );

    m_pGraphicsManager->DrawBatch( frame );
}