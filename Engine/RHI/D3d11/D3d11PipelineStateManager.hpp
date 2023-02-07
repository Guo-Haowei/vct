#pragma once
#include "Manager/PipelineStateManager.hpp"

struct D3d11PipelineState : public PipelineState {
    uint32_t shaderProgram = 0;
    D3d11PipelineState( PipelineState& rhs )
        : PipelineState( rhs ) {}
    D3d11PipelineState( PipelineState&& rhs )
        : PipelineState( std::move( rhs ) ) {}
};

class D3d11PipelineStateManager : public PipelineStateManager {
public:
    using PipelineStateManager::PipelineStateManager;
    virtual ~D3d11PipelineStateManager() = default;

protected:
    bool InitializePipelineState( PipelineState** ppPipelineState ) final;
    void DestroyPipelineState( PipelineState& pipelineState ) final;
};
