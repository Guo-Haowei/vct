#pragma once
#include "Core/BaseManager.hpp"

#include "PipelineStateManager.hpp"

struct GLFWwindow;

class GraphicsManager : _Inherits_ BaseManager {
public:
    GraphicsManager( const char* debugName )
        : BaseManager( debugName )
    {
    }

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& pipelineState ) {}
};

extern GraphicsManager* g_gfxMgr;
