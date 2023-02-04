#pragma once
#include <map>
#include <memory>
#include <string>

#include "Core/BaseManager.hpp"

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

class PipelineStateManager : _Inherits_ IRuntimeModule {
public:
    ~PipelineStateManager() override;

    virtual bool Init() override;
    virtual void Deinit() override;
    virtual void Tick() override {}

    bool RegisterPipelineState( PipelineState& pipelineState );
    void UnregisterPipelineState( PipelineState& pipelineState );
    void Clear();

    const std::shared_ptr<PipelineState> GetPipelineState(
        std::string name ) const;

protected:
    virtual bool InitializePipelineState( PipelineState** ppPipelineState )
    {
        return true;
    }
    virtual void DestroyPipelineState( PipelineState& pipelineState ) {}

protected:
    std::map<std::string, std::shared_ptr<PipelineState>> m_pipelineStates;
};

extern PipelineStateManager* g_pPipelineStateManager;