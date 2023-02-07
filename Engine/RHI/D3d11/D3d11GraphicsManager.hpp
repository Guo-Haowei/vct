#pragma once
#include "Manager/GraphicsManager.hpp"

#include <d3d11.h>
#include <dxgi.h>

struct ImmediateRenderTarget {
    ID3D11RenderTargetView* rtv;
    ID3D11DepthStencilView* dsv;
};

class D3d11GraphicsManager : public GraphicsManager {
public:
    virtual bool Initialize() override;
    virtual void Finalize() override;

    virtual void ResizeCanvas( int new_width, int new_height ) override;

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& pipelineState ) override;

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

    IDXGISwapChain* m_pSwapChain{ nullptr };
    ID3D11Device* m_pDevice{ nullptr };
    ID3D11DeviceContext* m_pCtx{ nullptr };

    IDXGIDevice* m_pDxgiDevice{ nullptr };
    IDXGIAdapter* m_pDxgiAdapter{ nullptr };
    IDXGIFactory* m_pDxgiFactory{ nullptr };
    DXGI_ADAPTER_DESC m_adapterDesc{};

    // render target
    ImmediateRenderTarget m_immediate;
};
