#pragma once
#include "Manager/PipelineStateManager.hpp"
#include <vector>

struct OpenGLPipelineState : public PipelineState {
    uint32_t shaderProgram = 0;
    OpenGLPipelineState( PipelineState& rhs )
        : PipelineState( rhs ) {}
    OpenGLPipelineState( PipelineState&& rhs )
        : PipelineState( std::move( rhs ) ) {}
};

class OpenGLPipelineStateManager : public PipelineStateManager {
public:
    using PipelineStateManager::PipelineStateManager;
    virtual ~OpenGLPipelineStateManager() = default;

protected:
    bool InitializePipelineState( PipelineState** ppPipelineState ) final;
    void DestroyPipelineState( PipelineState& pipelineState ) final;
};
