#include "D3d11PipelineStateManager.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <d3dcompiler.h>

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Manager/BaseApplication.hpp"

#include "D3d11GraphicsManager.hpp"

#include "Core/com_dvars.h"

#define HLSL_ROOT "Engine/Shader/HLSL/"

namespace fs = std::filesystem;
using std::string;

static bool CompileDXShaderFromFile( const std::string &shader_name, LPCSTR entry, LPCSTR target, ComPtr<ID3DBlob> &source_blob )
{
    fs::path sysPath = fs::path( Dvar_GetString( fs_base ) ) / HLSL_ROOT / shader_name;
    string shaderPath = sysPath.string() + ".hlsl";
    if ( !fs::exists( shaderPath ) ) {
        LOG_ERROR( "Shader '%s' does not exist", shaderPath.c_str() );
        return false;
    }

    std::wstring shaderPathW( shaderPath.begin(), shaderPath.end() );

    UINT flags = Dvar_GetBool( r_debug ) ? D3DCOMPILE_ENABLE_STRICTNESS : 0;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3DCompileFromFile(
        shaderPathW.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entry,
        target,
        flags,
        0,
        &source_blob,
        &errorBlob );

    if ( FAILED( hr ) ) {
        LOG_ERROR( "Failed to compile shader '%s'", shaderPath.c_str() );
        if ( errorBlob != nullptr ) {
            const char *details = (const char *)errorBlob->GetBufferPointer();
            LOG_FATAL( "Details:\n%s", details );
        }
        return false;
    }

    return true;
}

bool D3d11PipelineStateManager::InitializePipelineState( PipelineState **ppPipelineState )
{
    D3d11PipelineState *pnew_state = new D3d11PipelineState( **ppPipelineState );

    D3d11GraphicsManager *pGfxMgr = dynamic_cast<D3d11GraphicsManager *>( m_pApp->GetGraphicsManager() );
    ID3D11Device *pDevice = pGfxMgr->GetDevice();

    ComPtr<ID3DBlob> vsBlob;
    {
        const string &name = ( *ppPipelineState )->vertexShaderName;
        if ( !name.empty() ) {
            if ( !CompileDXShaderFromFile( name, "vs_main", "vs_5_0", vsBlob ) ) {
                return false;
            }
            DX_CALL( pDevice->CreateVertexShader(
                vsBlob->GetBufferPointer(),
                vsBlob->GetBufferSize(),
                NULL,
                &pnew_state->m_vs ) );
        }
    }
    {
        const string &name = ( *ppPipelineState )->geometryShaderName;
        ComPtr<ID3DBlob> blob;
        if ( !name.empty() ) {
            if ( !CompileDXShaderFromFile( name, "gs_main", "gs_5_0", blob ) ) {
                return false;
            }
            DX_CALL( pDevice->CreateGeometryShader(
                blob->GetBufferPointer(),
                blob->GetBufferSize(),
                NULL,
                &pnew_state->m_gs ) );
        }
    }
    {
        const string &name = ( *ppPipelineState )->pixelShaderName;
        ComPtr<ID3DBlob> blob;
        if ( !name.empty() ) {
            if ( !CompileDXShaderFromFile( name, "ps_main", "ps_5_0", blob ) ) {
                return false;
            }
            DX_CALL( pDevice->CreatePixelShader(
                blob->GetBufferPointer(),
                blob->GetBufferSize(),
                NULL,
                &pnew_state->m_ps ) );
        }
    }
    {
        const string &name = ( *ppPipelineState )->computeShaderName;
        ComPtr<ID3DBlob> blob;
        if ( !name.empty() ) {
            if ( !CompileDXShaderFromFile( name, "cs_main", "cs_5_0", blob ) ) {
                return false;
            }
            DX_CALL( pDevice->CreateComputeShader(
                blob->GetBufferPointer(),
                blob->GetBufferSize(),
                NULL,
                &pnew_state->m_cs ) );
        }
    }

    // input layout
    static const D3D11_INPUT_ELEMENT_DESC LAYOUT_SIMPLE[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    DX_CALL( pDevice->CreateInputLayout(
        LAYOUT_SIMPLE,
        array_length( LAYOUT_SIMPLE ),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &pnew_state->m_layout ) );

    // pipeline state
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.FrontCounterClockwise = true;
        switch ( ( *ppPipelineState )->cullFaceMode ) {
            case CULL_FACE_MODE::NONE:
                desc.CullMode = D3D11_CULL_NONE;
                break;
            case CULL_FACE_MODE::FRONT:
                desc.CullMode = D3D11_CULL_FRONT;
                break;
            case CULL_FACE_MODE::BACK:
                desc.CullMode = D3D11_CULL_BACK;
                break;
        }
        pDevice->CreateRasterizerState( &desc, &pnew_state->m_rs );
    }

    *ppPipelineState = pnew_state;
    return true;
}

void D3d11PipelineStateManager::DestroyPipelineState( PipelineState &pipelineState )
{
    D3d11PipelineState *pPipelineState = dynamic_cast<D3d11PipelineState *>( &pipelineState );

    SafeRelease( pPipelineState->m_vs );
    SafeRelease( pPipelineState->m_gs );
    SafeRelease( pPipelineState->m_ps );
    SafeRelease( pPipelineState->m_cs );
    SafeRelease( pPipelineState->m_layout );
}
