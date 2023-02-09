#pragma once
#include "Manager/GraphicsManager.hpp"

#include <dxgi.h>
#include <d3d11.h>

#include "RHI/D3dCommon.hpp"

struct ImmediateRenderTarget {
    ID3D11RenderTargetView* rtv;
    ID3D11DepthStencilView* dsv;
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
        uint32_t index_count{ 0 };
        size_t index_offset{ 0 };
        uint32_t property_count{ 0 };
        size_t property_offset{ 0 };
        // size_t cbv_srv_uav_offset{ 0 };
    };

    friend class D3d11PipelineStateManager;
};
