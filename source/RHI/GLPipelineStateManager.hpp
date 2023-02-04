#pragma once
#include "Graphics/PipelineStateManager.hpp"

struct GLPipelineState : public PipelineState {
    uint32_t shaderProgram = 0;
    GLPipelineState( PipelineState& rhs )
        : PipelineState( rhs ) {}
    GLPipelineState( PipelineState&& rhs )
        : PipelineState( std::move( rhs ) ) {}
};

class GLPipelineStateManager : public PipelineStateManager {
public:
    using PipelineStateManager::PipelineStateManager;
    virtual ~GLPipelineStateManager() = default;

protected:
    bool InitializePipelineState( PipelineState** ppPipelineState ) final;
    void DestroyPipelineState( PipelineState& pipelineState ) final;
};
