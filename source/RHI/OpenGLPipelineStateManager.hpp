#pragma once
#include "Graphics/PipelineStateManager.hpp"

struct OpenGLPipelineState : public PipelineState {
    uint32_t shaderProgram = 0;
    OpenGLPipelineState( PipelineState& rhs )
        : PipelineState( rhs ) {}
    OpenGLPipelineState( PipelineState&& rhs )
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
