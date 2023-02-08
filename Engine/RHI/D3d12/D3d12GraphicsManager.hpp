#pragma once
#include "Manager/GraphicsManager.hpp"

#include <dxgi1_4.h>
#include <d3d12.h>

#include "RHI/D3dCommon.hpp"

struct FrameContext {
    ID3D12CommandAllocator* CommandAllocator;
    UINT64 FenceValue;
};

class D3d12GraphicsManager : public GraphicsManager {
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
    bool CreateDevice();
    bool CreateObjects();

    bool CreateRenderTarget();
    void DestroyRenderTarget();

    void WaitForLastSubmittedFrame();
    FrameContext* WaitForNextFrameResources();

    ID3D12Device* GetDevice() { return m_pDevice.Get(); }

    ComPtr<ID3D12Device> m_pDevice;
    ComPtr<IDXGISwapChain3> m_pSwapChain;

    // @TODO: refactor
    static constexpr int NUM_FRAMES_IN_FLIGHT = 3;
    static constexpr int NUM_BACK_BUFFERS = 3;
    FrameContext g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
    uint32_t g_frameIndex = 0;

    ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
    ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
    ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
    ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
    ID3D12Fence* g_fence = NULL;
    HANDLE g_fenceEvent = NULL;
    UINT64 g_fenceLastSignaledValue = 0;
    HANDLE g_hSwapChainWaitableObject = NULL;
    ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
    D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

    friend class D3d12PipelineStateManager;
};
