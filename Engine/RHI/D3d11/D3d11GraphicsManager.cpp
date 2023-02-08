#include "D3d11GraphicsManager.hpp"

#include <d3d11.h>

#include "D3d11PipelineStateManager.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Core/com_dvars.h"
#include "Core/GlfwApplication.hpp"

#include "imgui_impl_dx11.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "Manager/BaseApplication.hpp"
#include "Manager/SceneManager.hpp"

static ID3D11Buffer *vertex_buffer_ptr = NULL;

bool D3d11GraphicsManager::Initialize()
{
    if ( !GraphicsManager::Initialize() ) {
        return false;
    }

    if ( !CreateDeviceAndSwapChain() ) {
        return false;
    }

    if ( !CreateRenderTarget() ) {
        return false;
    }

    if ( !ImGui_ImplDX11_Init( m_pDevice.Get(), m_pCtx.Get() ) ) {
        LOG_FATAL( "D3d11GraphicsManager::Initialize: ImGui_ImplDX11_Init() failed!" );
        return false;
    }

    if ( !ImGui_ImplDX11_CreateDeviceObjects() ) {
        LOG_FATAL( "D3d11GraphicsManager::Initialize: ImGui_ImplDX11_CreateDeviceObjects() failed!" );
        return false;
    }

    auto pipelineStateManager = dynamic_cast<BaseApplication *>( m_pApp )->GetPipelineStateManager();
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new GuiPass( this, pipelineStateManager, nullptr, 0 ) ) );

    // @TODO: temp
    { /*** load mesh data into vertex buffer **/
        float vertex_data_array[] = {
            0.0f, 0.5f, 0.0f,    // point at top
            0.5f, -0.5f, 0.0f,   // point at bottom-right
            -0.5f, -0.5f, 0.0f,  // point at bottom-left
        };

        D3D11_BUFFER_DESC vertex_buff_descr = {};
        vertex_buff_descr.ByteWidth = sizeof( vertex_data_array );
        vertex_buff_descr.Usage = D3D11_USAGE_DEFAULT;
        vertex_buff_descr.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA sr_data = { 0 };
        sr_data.pSysMem = vertex_data_array;
        HRESULT hr = m_pDevice->CreateBuffer(
            &vertex_buff_descr,
            &sr_data,
            &vertex_buffer_ptr );
        assert( SUCCEEDED( hr ) );
    }

    return ( m_bInitialized = true );
}

void D3d11GraphicsManager::Finalize()
{
    ImGui_ImplDX11_Shutdown();
}

void D3d11GraphicsManager::ResizeCanvas( int new_width, int new_height )
{
    DestroyRenderTarget();
    DX_CALL( m_pSwapChain->ResizeBuffers( 0, new_width, new_height, DXGI_FORMAT_UNKNOWN, 0 ) );
    CreateRenderTarget();
}

void D3d11GraphicsManager::SetPipelineState( const std::shared_ptr<PipelineState> &pipeline_state )
{
    const D3d11PipelineState *pPipelineState = dynamic_cast<const D3d11PipelineState *>( pipeline_state.get() );

    auto &ctx = m_pCtx;
    if ( pipeline_state->pipelineType == PIPELINE_TYPE::GRAPHIC ) {
        ASSERT( pPipelineState->m_vs && pPipelineState->m_ps );
        ctx->VSSetShader( pPipelineState->m_vs, 0, 0 );
        ctx->PSSetShader( pPipelineState->m_ps, 0, 0 );
        if ( pPipelineState->m_gs ) {
            ctx->GSSetShader( pPipelineState->m_gs, 0, 0 );
        }
    }
    else {
        ASSERT( 0 && "TODO" );
    }

    ctx->IASetInputLayout( pPipelineState->m_layout );
    ctx->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // TODO: modes
}

void D3d11GraphicsManager::DrawBatch( const Frame & )
{
    // @TODO: culling
    const Frame &frame = m_frame;
    //for ( auto &pDbc : frame.batchContexts ) {
    //    SetPerBatchConstants( *pDbc );

    //    const auto &dbc = dynamic_cast<const OpenGLDrawBatchContext &>( *pDbc );

    //    const MaterialData *matData = reinterpret_cast<MaterialData *>( pDbc->pEntity->m_material->gpuResource );

    //    FillMaterialCB( matData, g_materialCache.cache );
    //    g_materialCache.Update();

    //    glBindVertexArray( dbc.vao );
    //    glDrawElements( dbc.mode, dbc.count, dbc.type, nullptr );
    //}

    //glBindVertexArray( 0 );
}

void D3d11GraphicsManager::SetPerFrameConstants( const DrawFrameContext &context )
{
    // glBindBuffer( GL_UNIFORM_BUFFER, m_uboDrawFrameConstant );

    // const auto &constants = static_cast<const PerFrameConstants &>( context );

    // glBufferData( GL_UNIFORM_BUFFER, kSizePerFrameConstantBuffer, &constants, GL_DYNAMIC_DRAW );

    // glBindBuffer( GL_UNIFORM_BUFFER, 0 );
}

void D3d11GraphicsManager::SetPerBatchConstants( const DrawBatchContext &context )
{
    // glBindBuffer( GL_UNIFORM_BUFFER, m_uboDrawBatchConstant );

    // const auto &constant = static_cast<const PerBatchConstants &>( context );

    // glBufferData( GL_UNIFORM_BUFFER, kSizePerBatchConstantBuffer, &constant, GL_DYNAMIC_DRAW );

    // glBindBuffer( GL_UNIFORM_BUFFER, 0 );
}

void D3d11GraphicsManager::InitializeGeometries( const Scene &scene )
{
    //uint32_t batch_index = 0;
    //for ( const auto &entity : scene.m_entities ) {
    //    if ( !( entity->m_flag & Entity::FLAG_GEOMETRY ) ) {
    //        continue;
    //    }

    //    const MeshData *drawData = reinterpret_cast<MeshData *>( entity->m_mesh->gpuResource );

    //    auto dbc = std::make_shared<OpenGLDrawBatchContext>();
    //    dbc->batchIndex = batch_index++;
    //    dbc->vao = drawData->vao;
    //    dbc->mode = GL_TRIANGLES;
    //    dbc->type = GL_UNSIGNED_INT;
    //    dbc->count = drawData->count;

    //    dbc->pEntity = entity.get();
    //    dbc->Model = mat4( 1 );

    //    m_frame.batchContexts.push_back( dbc );
    //}

    //auto createUBO = []( int slot ) {
    //    GLuint handle = 0;
    //    glGenBuffers( 1, &handle );
    //    glBindBufferBase( GL_UNIFORM_BUFFER, slot, handle );
    //    glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    //    return handle;
    //};
    //m_uboDrawFrameConstant = createUBO( 0 );
    //m_uboDrawBatchConstant = createUBO( 1 );
}

void D3d11GraphicsManager::BeginFrame( Frame &frame )
{
    GraphicsManager::BeginFrame( frame );

    //SetPerFrameConstants( frame.frameContexts );
}

void D3d11GraphicsManager::EndFrame( Frame &frame )
{
    auto &ctx = m_pCtx;
    int w, h;
    m_pApp->GetFramebufferSize( w, h );
    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)w, (FLOAT)h, 0.0f, 1.0f };
    ctx->RSSetViewports( 1, &viewport );

    /**** Output Merger *****/
    ctx->OMSetRenderTargets( 1, &m_immediate.rtv, NULL );
    const float clearColor[4] = { 0.3f, 0.4f, 0.3f, 1.0f };
    ctx->ClearRenderTargetView( m_immediate.rtv, clearColor );

    /***** Input Assembler (map how the vertex shader inputs should be read from vertex buffer) ******/
    IPipelineStateManager *pPipelineStateManager = dynamic_cast<BaseApplication *>( m_pApp )->GetPipelineStateManager();
    auto PSO = pPipelineStateManager->GetPipelineState( "SIMPLE" );
    SetPipelineState( PSO );

    /*** draw the vertex buffer with the shaders ****/
    UINT vertex_stride = 3 * sizeof( float );
    UINT vertex_offset = 0;
    m_pCtx->IASetVertexBuffers( 0, 1, &vertex_buffer_ptr, &vertex_stride, &vertex_offset );
    m_pCtx->Draw( 3, 0 );

    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

    GraphicsManager::EndFrame( frame );
}

void D3d11GraphicsManager::Present()
{
    m_pSwapChain->Present( 1, 0 );
}

bool D3d11GraphicsManager::CreateDeviceAndSwapChain()
{
    GLFWwindow *pGlfwWindow = reinterpret_cast<GLFWwindow *>( m_pApp->GetMainWindowHandler() );
    // create device and swap chain
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = glfwGetWin32Window( pGlfwWindow );
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = Dvar_GetBool( r_debug ) ? D3D11_CREATE_DEVICE_DEBUG : 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[] = { D3D_FEATURE_LEVEL_11_0 };

    DX_CALL( D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        createDeviceFlags,
        featureLevelArray,
        array_length( featureLevelArray ),
        D3D11_SDK_VERSION,
        &sd,
        &m_pSwapChain,
        &m_pDevice,
        &featureLevel,
        &m_pCtx ) );

    DX_CALL( m_pDevice->QueryInterface( IID_PPV_ARGS( &m_pDxgiDevice ) ) );
    DX_CALL( m_pDxgiDevice->GetParent( IID_PPV_ARGS( &m_pDxgiAdapter ) ) );
    DX_CALL( m_pDxgiAdapter->GetParent( IID_PPV_ARGS( &m_pDxgiFactory ) ) );

    return true;
}

bool D3d11GraphicsManager::CreateRenderTarget()
{
    ComPtr<ID3D11Texture2D> backbuffer;
    m_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &backbuffer ) );
    DX_CALL( m_pDevice->CreateRenderTargetView( backbuffer.Get(), nullptr, &m_immediate.rtv ) );

    D3D11_TEXTURE2D_DESC backBufferDesc;
    backbuffer->GetDesc( &backBufferDesc );

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = backBufferDesc.Width;
    desc.Height = backBufferDesc.Height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ComPtr<ID3D11Texture2D> depthBuffer;

    DX_CALL( m_pDevice->CreateTexture2D( &desc, 0, &depthBuffer ) );
    DX_CALL( m_pDevice->CreateDepthStencilView( depthBuffer.Get(), nullptr, &m_immediate.dsv ) );
    return true;
}

void D3d11GraphicsManager::DestroyRenderTarget()
{
    SafeRelease( m_immediate.rtv );
    SafeRelease( m_immediate.dsv );
}