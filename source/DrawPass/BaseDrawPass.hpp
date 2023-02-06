#pragma once
#include "Graphics/FrameStructure.hpp"
#include "Interface/IGraphicsManager.hpp"
#include "Interface/IPipelineStateManager.hpp"

// @TODO: refactor
#include "Graphics/r_rendertarget.h"

enum CLEAR_FLAG : uint32_t {
    CLEAR_FLAG_NONE = 0,
    CLEAR_FLAG_COLOR = 1,
    CLEAR_FLAG_DEPTH = 2,
    CLEAR_FLAG_STENCIL = 4,
    CLEAR_FLAG_COLOR_DPETH = CLEAR_FLAG_COLOR | CLEAR_FLAG_DEPTH,
    CLEAR_FLAG_DEPTH_STENCIL = CLEAR_FLAG_DEPTH | CLEAR_FLAG_STENCIL,
    CLEAR_FLAG_ALL = CLEAR_FLAG_COLOR | CLEAR_FLAG_DEPTH | CLEAR_FLAG_STENCIL,
};

class BaseDrawPass {
public:
    BaseDrawPass( IGraphicsManager* pGfxMgr, IPipelineStateManager* pPipeMgr,
                  RenderTarget* pRt, uint32_t clearFlag );

    virtual ~BaseDrawPass() = default;

    virtual void BeginPass( Frame& frame );
    virtual void Draw( Frame& frame );
    virtual void EndPass( Frame& frame );

protected:
    // std::vector<std::shared_ptr<IDrawSubPass>> m_DrawSubPasses;
    IGraphicsManager* m_pGraphicsManager;
    IPipelineStateManager* m_pPipelineStateManager;

    bool m_bClearRT = false;

    RenderTarget* m_pRenderTarget;
    const uint32_t m_clearFlag;
};
