#include "D3d12GraphicsManager.hpp"

#include "D3d12PipelineStateManager.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Core/com_dvars.h"
#include "Core/GlfwApplication.hpp"

#include "imgui_impl_dx12.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "Manager/BaseApplication.hpp"
#include "Manager/SceneManager.hpp"

void WaitForLastSubmittedFrame();
FrameContext *WaitForNextFrameResources();

bool D3d12GraphicsManager::Initialize()
{
    if ( !GraphicsManager::Initialize() ) {
        return false;
    }

    if ( !CreateDevice() ) {
        return false;
    }

    if ( !CreateObjects() ) {
        return false;
    }

    if ( !CreateRenderTarget() ) {
        return false;
    }

    if ( !ImGui_ImplDX12_Init( m_pDevice.Get(), NUM_FRAMES_IN_FLIGHT,
                               DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
                               g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                               g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart() ) ) {
        LOG_FATAL( "D3d12GraphicsManager::Initialize: ImGui_ImplDX12_Init() failed!" );
        return false;
    }

    if ( !ImGui_ImplDX12_CreateDeviceObjects() ) {
        LOG_FATAL( "D3d12GraphicsManager::Initialize: ImGui_ImplDX12_CreateDeviceObjects() failed!" );
        return false;
    }

    auto pipelineStateManager = m_pApp->GetPipelineStateManager();
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new GuiPass( this, pipelineStateManager, nullptr, 0 ) ) );

    return ( m_bInitialized = true );
}

void D3d12GraphicsManager::Finalize()
{
    WaitForLastSubmittedFrame();
    ImGui_ImplDX12_Shutdown();
}

void D3d12GraphicsManager::ResizeCanvas( int new_width, int new_height )
{
    DestroyRenderTarget();
    DX_CALL( m_pSwapChain->ResizeBuffers( 0, new_width, new_height, DXGI_FORMAT_UNKNOWN, 0 ) );
    CreateRenderTarget();
}

void D3d12GraphicsManager::SetPipelineState( const std::shared_ptr<PipelineState> &pipeline_state )
{
    const D3d12PipelineState *pPipelineState = dynamic_cast<const D3d12PipelineState *>( pipeline_state.get() );
}

void D3d12GraphicsManager::DrawBatch( const Frame & )
{
}

void D3d12GraphicsManager::SetPerFrameConstants( const DrawFrameContext &context )
{
}

void D3d12GraphicsManager::SetPerBatchConstants( const DrawBatchContext &context )
{
}

void D3d12GraphicsManager::InitializeGeometries( const Scene &scene )
{
}

void D3d12GraphicsManager::BeginFrame( Frame &frame )
{
    GraphicsManager::BeginFrame( frame );
}

void D3d12GraphicsManager::EndFrame( Frame &frame )
{
    FrameContext *frameCtx = WaitForNextFrameResources();
    UINT backBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
    frameCtx->CommandAllocator->Reset();

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    g_pd3dCommandList->Reset( frameCtx->CommandAllocator, NULL );
    g_pd3dCommandList->ResourceBarrier( 1, &barrier );

    // Render Dear ImGui graphics
    const float clear_color_with_alpha[4] = { 0.3f, 0.3f, 0.4f, 1.0f };
    g_pd3dCommandList->ClearRenderTargetView( g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL );
    g_pd3dCommandList->OMSetRenderTargets( 1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL );
    g_pd3dCommandList->SetDescriptorHeaps( 1, &g_pd3dSrvDescHeap );
    ImGui_ImplDX12_RenderDrawData( ImGui::GetDrawData(), g_pd3dCommandList );
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    g_pd3dCommandList->ResourceBarrier( 1, &barrier );
    g_pd3dCommandList->Close();

    g_pd3dCommandQueue->ExecuteCommandLists( 1, (ID3D12CommandList *const *)&g_pd3dCommandList );

    UINT64 fenceValue = g_fenceLastSignaledValue + 1;
    g_pd3dCommandQueue->Signal( g_fence, fenceValue );
    g_fenceLastSignaledValue = fenceValue;
    frameCtx->FenceValue = fenceValue;

    GraphicsManager::EndFrame( frame );
}

void D3d12GraphicsManager::Present()
{
    m_pSwapChain->Present( 1, 0 );
}

bool D3d12GraphicsManager::CreateDevice()
{
    const bool enableDebugLayer = Dvar_GetBool( r_debug );
    ID3D12Debug *pD3d12Debug = nullptr;
    if ( enableDebugLayer ) {
        if ( SUCCEEDED( D3D12GetDebugInterface( IID_PPV_ARGS( &pD3d12Debug ) ) ) ) {
            pD3d12Debug->EnableDebugLayer();
        }
    }

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if ( FAILED( DX_CALL( D3D12CreateDevice( nullptr, featureLevel, IID_PPV_ARGS( &m_pDevice ) ) ) ) ) {
        return false;
    }

    if ( pD3d12Debug ) {
        ID3D12InfoQueue *pInfoQueue = NULL;
        m_pDevice->QueryInterface( IID_PPV_ARGS( &pInfoQueue ) );
        pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR, true );
        pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, true );
        pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_WARNING, true );
        pInfoQueue->Release();
        pD3d12Debug->Release();
    }

    return true;
}

bool D3d12GraphicsManager::CreateObjects()
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd{ 0 };
    {
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

    ///////////////

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if ( FAILED( DX_CALL( m_pDevice->CreateDescriptorHeap( &desc, IID_PPV_ARGS( &g_pd3dRtvDescHeap ) ) ) ) ) {
            return false;
        }

        SIZE_T rtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for ( UINT i = 0; i < NUM_BACK_BUFFERS; i++ ) {
            g_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if ( FAILED( DX_CALL( m_pDevice->CreateDescriptorHeap( &desc, IID_PPV_ARGS( &g_pd3dSrvDescHeap ) ) ) ) ) {
            return false;
        }
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if ( FAILED( DX_CALL( m_pDevice->CreateCommandQueue( &desc, IID_PPV_ARGS( &g_pd3dCommandQueue ) ) ) ) ) {
            return false;
        }
    }

    for ( UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++ ) {
        if ( FAILED( DX_CALL( m_pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &g_frameContext[i].CommandAllocator ) ) ) ) ) {
            return false;
        }
    }

    if ( FAILED( DX_CALL( m_pDevice->CreateCommandList( 0,
                                                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                        g_frameContext[0].CommandAllocator,
                                                        nullptr,
                                                        IID_PPV_ARGS( &g_pd3dCommandList ) ) ) ) ||
         FAILED( DX_CALL( g_pd3dCommandList->Close() ) ) ) {
        return false;
    }

    if ( FAILED( DX_CALL( m_pDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &g_fence ) ) ) ) ) {
        return false;
    }

    g_fenceEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if ( g_fenceEvent == NULL ) {
        LOG_FATAL( "Failed to create fence" );
        return false;
    }
    {
        IDXGIFactory4 *dxgiFactory = nullptr;
        IDXGISwapChain1 *swapChain1 = nullptr;
        if ( FAILED( DX_CALL( CreateDXGIFactory1( IID_PPV_ARGS( &dxgiFactory ) ) ) ) ) {
            return false;
        }
        GLFWwindow *pGlfwWindow = reinterpret_cast<GLFWwindow *>( m_pApp->GetMainWindowHandler() );
        if ( FAILED( DX_CALL( dxgiFactory->CreateSwapChainForHwnd(
                 g_pd3dCommandQueue,
                 glfwGetWin32Window( pGlfwWindow ),
                 &sd, nullptr, nullptr, &swapChain1 ) ) ) ) {
            return false;
        }

        if ( FAILED( DX_CALL( swapChain1->QueryInterface( IID_PPV_ARGS( &m_pSwapChain ) ) ) ) ) {
            return false;
        }

        SafeRelease( swapChain1 );
        SafeRelease( dxgiFactory );
        m_pSwapChain->SetMaximumFrameLatency( NUM_BACK_BUFFERS );
        g_hSwapChainWaitableObject = m_pSwapChain->GetFrameLatencyWaitableObject();
    }

    return true;
}

bool D3d12GraphicsManager::CreateRenderTarget()
{
    for ( int i = 0; i < NUM_BACK_BUFFERS; i++ ) {
        ID3D12Resource *pBackBuffer = nullptr;
        if ( FAILED( m_pSwapChain->GetBuffer( i, IID_PPV_ARGS( &pBackBuffer ) ) ) ) {
            return false;
        }
        m_pDevice->CreateRenderTargetView( pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i] );
        g_mainRenderTargetResource[i] = pBackBuffer;
    };
    return true;
}

void D3d12GraphicsManager::DestroyRenderTarget()
{
    WaitForLastSubmittedFrame();

    for ( UINT i = 0; i < NUM_BACK_BUFFERS; i++ ) {
        SafeRelease( g_mainRenderTargetResource[i] );
    }
}

void D3d12GraphicsManager::WaitForLastSubmittedFrame()
{
    FrameContext *frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if ( fenceValue == 0 ) {
        return;  // No fence was signaled
    }

    frameCtx->FenceValue = 0;
    if ( g_fence->GetCompletedValue() >= fenceValue ) {
        return;
    }

    g_fence->SetEventOnCompletion( fenceValue, g_fenceEvent );
    WaitForSingleObject( g_fenceEvent, INFINITE );
}

FrameContext *D3d12GraphicsManager::WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
    DWORD numWaitableObjects = 1;

    FrameContext *frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if ( fenceValue != 0 )  // means no fence was signaled
    {
        frameCtx->FenceValue = 0;
        g_fence->SetEventOnCompletion( fenceValue, g_fenceEvent );
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects( numWaitableObjects, waitableObjects, TRUE, INFINITE );

    return frameCtx;
}