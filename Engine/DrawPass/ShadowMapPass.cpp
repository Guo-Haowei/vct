#include "ShadowMapPass.hpp"

#include "Core/com_dvars.h"

void ShadowMapPass::Draw( Frame& frame )
{
    auto PSO = m_pPipelineStateManager->GetPipelineState( "SHADOW" );
    m_pGraphicsManager->SetPipelineState( PSO );

    const int shadowRes = Dvar_GetInt( r_shadowRes );
    glViewport( 0, 0, shadowRes, shadowRes );

    m_pGraphicsManager->DrawBatch( frame );
}