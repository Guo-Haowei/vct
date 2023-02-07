#include "D3d11PipelineStateManager.hpp"

#include <sstream>
#include <vector>

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Interface/IAssetLoader.hpp"
#include "Manager/BaseApplication.hpp"

bool D3d11PipelineStateManager::InitializePipelineState( PipelineState **ppPipelineState )
{
    D3d11PipelineState *pnew_state = new D3d11PipelineState( **ppPipelineState );
    // ShaderSourceList list;

    // if ( !( *ppPipelineState )->vertexShaderName.empty() ) {
    //     list.emplace_back( GL_VERTEX_SHADER, ( *ppPipelineState )->vertexShaderName );
    // }

    // if ( !( *ppPipelineState )->pixelShaderName.empty() ) {
    //     list.emplace_back( GL_FRAGMENT_SHADER, ( *ppPipelineState )->pixelShaderName );
    // }

    // if ( !( *ppPipelineState )->geometryShaderName.empty() ) {
    //     list.emplace_back( GL_GEOMETRY_SHADER, ( *ppPipelineState )->geometryShaderName );
    // }

    // if ( !( *ppPipelineState )->computeShaderName.empty() ) {
    //     list.emplace_back( GL_COMPUTE_SHADER, ( *ppPipelineState )->computeShaderName );
    // }

    // auto assetLoader = dynamic_cast<BaseApplication *>( m_pApp )->GetAssetLoader();
    // bool result = LoadShaderProgram( list, pnew_state->shaderProgram, assetLoader );

    bool result = true;
    *ppPipelineState = pnew_state;
    return result;
}

void D3d11PipelineStateManager::DestroyPipelineState( PipelineState &pipelineState )
{
    D3d11PipelineState *pPipelineState = dynamic_cast<D3d11PipelineState *>( &pipelineState );
    // @TODO: ...
}
