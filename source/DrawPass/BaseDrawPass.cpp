#include "BaseDrawPass.hpp"

#include "Base/Asserts.h"

#include "glad/glad.h"

BaseDrawPass::BaseDrawPass( IGraphicsManager* pGfxMgr, IPipelineStateManager* pPipeMgr,
                            RenderTarget* pRt, uint32_t clearFlag )
    : m_pGraphicsManager( pGfxMgr ), m_pPipelineStateManager( pPipeMgr ), m_pRenderTarget( pRt ), m_clearFlag( clearFlag )
{
    ASSERTRANGE( m_clearFlag, CLEAR_FLAG_NONE, CLEAR_FLAG_ALL );
}

void BaseDrawPass::BeginPass( Frame& frame )
{
    if ( m_pRenderTarget ) {
        m_pRenderTarget->Bind();
    }
    if ( m_clearFlag ) {
        int flag = 0;
        if ( m_clearFlag & CLEAR_FLAG_COLOR ) {
            flag |= GL_COLOR_BUFFER_BIT;
        }
        if ( m_clearFlag & CLEAR_FLAG_DEPTH ) {
            flag |= GL_DEPTH_BUFFER_BIT;
        }
        if ( m_clearFlag & CLEAR_FLAG_STENCIL ) {
            flag |= GL_STENCIL_BUFFER_BIT;
        }
        glClear( flag );
    }
}

void BaseDrawPass::EndPass( Frame& )
{
    if ( m_pRenderTarget ) {
        m_pRenderTarget->Unbind();
    }
}

void BaseDrawPass::Draw( Frame& frame )
{
    // DO NOTHING FOR NOW
}
