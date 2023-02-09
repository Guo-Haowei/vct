#pragma once
#include "Manager/GraphicsManager.hpp"

#include <dxgi.h>
#include <d3d11.h>

#include "RHI/D3dCommon.hpp"

struct D3dMeshData {
    ID3D11Buffer* indexBuffer{ nullptr };
    ID3D11Buffer* positionBuffer{ nullptr };
    ID3D11Buffer* normalBuffer{ nullptr };
    uint32_t indexCount{ 0 };
};

struct ImmediateRenderTarget {
    ID3D11RenderTargetView* rtv{ nullptr };
    ID3D11DepthStencilView* dsv{ nullptr };
};

class D3d11GraphicsManager : public GraphicsManager {
public:
    virtual bool Initialize() override;
    virtual void Finalize() override;

    virtual void ResizeCanvas( int new_width, int new_height ) override;

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& pipeline_state ) override;

    virtual void DrawBatch( const Frame& frame ) override;

    virtual void InitializeGeometries( const Scene& scene ) override;

    virtual void BeginFrame( Frame& frame ) override;
    virtual void EndFrame( Frame& frame ) override;

    virtual void Present() override;

protected:
    void SetPerFrameConstants( const DrawFrameContext& context );
    void SetPerBatchConstants( const DrawBatchContext& context );

private:
    bool CreateDeviceAndSwapChain();

    bool CreateRenderTarget();
    void DestroyRenderTarget();

    ID3D11Device* GetDevice() { return m_pDevice.Get(); }

    ComPtr<IDXGISwapChain> m_pSwapChain;
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pCtx;

    ComPtr<IDXGIDevice> m_pDxgiDevice;
    ComPtr<IDXGIAdapter> m_pDxgiAdapter;
    ComPtr<IDXGIFactory> m_pDxgiFactory;

    DXGI_ADAPTER_DESC m_adapterDesc{};

    // render target
    ImmediateRenderTarget m_immediate;

    ID3D11Buffer* m_drawFrameConstant{ nullptr };
    ID3D11Buffer* m_drawBatchConstant{ nullptr };

    struct D3dDrawBatchContext : public DrawBatchContext {
        uint32_t indexCount{ 0 };
        ID3D11Buffer* indexBuffer;
        ID3D11Buffer* positionBuffer;
        ID3D11Buffer* normalBuffer;
    };

    std::vector<std::shared_ptr<D3dMeshData>> m_sceneMeshData;

    friend class D3d11PipelineStateManager;
};
