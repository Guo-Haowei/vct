#pragma once

#include <map>
#include <memory>
#include <string>

#include "IRuntimeModule.hpp"

enum class PIPELINE_TYPE {
    GRAPHIC,
    COMPUTE,
};

enum class DEPTH_TEST_MODE {
    NONE,
    LARGE,
    LARGE_EQUAL,
    EQUAL,
    LESS_EQUAL,
    LESS,
    NOT_EQUAL,
    NEVER,
    ALWAYS,
};

enum class CULL_FACE_MODE {
    NONE,
    FRONT,
    BACK
};

enum class STENCIL_TEST_MODE {
    NONE,
};

struct PipelineState {
    virtual ~PipelineState() = default;

    std::string pipelineStateName;
    PIPELINE_TYPE pipelineType;

    std::string vertexShaderName;
    std::string pixelShaderName;
    std::string computeShaderName;
    std::string geometryShaderName;

    DEPTH_TEST_MODE depthTestMode{ DEPTH_TEST_MODE::ALWAYS };
    STENCIL_TEST_MODE stencilTestMode{ STENCIL_TEST_MODE::NONE };
    CULL_FACE_MODE cullFaceMode{ CULL_FACE_MODE::BACK };
};

class IPipelineStateManager : public IRuntimeModule {
public:
    virtual bool RegisterPipelineState( PipelineState& pipelineState ) = 0;
    virtual void UnregisterPipelineState( PipelineState& pipelineState ) = 0;
    virtual void Clear() = 0;

    virtual const std::shared_ptr<PipelineState> GetPipelineState( const std::string& name ) const = 0;
};
