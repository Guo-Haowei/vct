#pragma once
#include <d3d11.h>

#include "Manager/PipelineStateManager.hpp"
#include "RHI/D3dCommon.hpp"

struct D3d11PipelineState : public PipelineState {
    D3d11PipelineState( PipelineState& rhs )
        : PipelineState( rhs ) {}
    D3d11PipelineState( PipelineState&& rhs )
        : PipelineState( std::move( rhs ) ) {}

    ID3D11VertexShader* m_vs{ nullptr };
    ID3D11GeometryShader* m_gs{ nullptr };
    ID3D11PixelShader* m_ps{ nullptr };
    ID3D11ComputeShader* m_cs{ nullptr };
    ID3D11InputLayout* m_layout{ nullptr };
};

class D3d11PipelineStateManager : public PipelineStateManager {
public:
    using PipelineStateManager::PipelineStateManager;

    virtual ~D3d11PipelineStateManager() = default;

protected:
    bool InitializePipelineState( PipelineState** ppPipelineState ) final;
    void DestroyPipelineState( PipelineState& pipelineState ) final;
};
