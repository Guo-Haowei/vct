#pragma once

#include "Manager/PipelineStateManager.hpp"
#include "RHI/D3dCommon.hpp"

struct D3d12PipelineState : public PipelineState {
    D3d12PipelineState( PipelineState& rhs )
        : PipelineState( rhs ) {}
    D3d12PipelineState( PipelineState&& rhs )
        : PipelineState( std::move( rhs ) ) {}
};

class D3d12PipelineStateManager : public PipelineStateManager {
public:
    using PipelineStateManager::PipelineStateManager;

    virtual ~D3d12PipelineStateManager() = default;

protected:
    bool InitializePipelineState( PipelineState** ppPipelineState ) final;
    void DestroyPipelineState( PipelineState& pipelineState ) final;
};
