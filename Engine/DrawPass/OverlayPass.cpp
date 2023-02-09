#include "OverlayPass.hpp"

#include "Graphics/gl_utils.h"

#include "Manager/BaseApplication.hpp"

#include "RHI/OpenGL/OpenGLGraphicsManager.hpp"

void OverlayPass::Draw( Frame& frame )
{
    unused( frame );

    auto PSO = m_pPipelineStateManager->GetPipelineState( "OVERLAY" );
    m_pGraphicsManager->SetPipelineState( PSO );

    auto pApp = m_pPipelineStateManager->GetAppPointer();
    int width = 0, height = 0;
    pApp->GetFramebufferSize( width, height );
    glViewport( 0, 0, width, height );

    glBindVertexArray( g_quad.vao );
    glDrawArraysInstanced( GL_TRIANGLES, 0, 6, 1 );
}