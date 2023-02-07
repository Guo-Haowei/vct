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
    using ShaderSourceList = std::vector<std::pair<uint32_t, std::string>>;

public:
    using PipelineStateManager::PipelineStateManager;
    virtual ~OpenGLPipelineStateManager() = default;

protected:
    bool InitializePipelineState( PipelineState** ppPipelineState ) final;
    void DestroyPipelineState( PipelineState& pipelineState ) final;

private:
    std::string ProcessShader( const std::string& source );
    bool LoadShaderFromFile( const char* file, const uint32_t shaderType, uint32_t& shader );
    bool LoadShaderProgram( const ShaderSourceList& source, uint32_t& shaderProgram );
};
