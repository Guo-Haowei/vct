#include "PipelineStateManager.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#define VS_HUD_LINE3D_SOURCE_FILE       "hud/line3d.vert"
#define PS_HUD_LINE3D_SOURCE_FILE       "hud/line3d.frag"
#define VS_HUD_IMAGE_SOURCE_FILE        "hud/image.vert"
#define PS_HUD_IMAGE_SOURCE_FILE        "hud/image.frag"
#define VS_HUD_TEXTURE_SOURCE_FILE      "fullscreen.vert"
#define PS_HUD_TEXTURE_SOURCE_FILE      "hud/texture.frag"
#define VS_DEPTH_SOURCE_FILE            "depth.vert"
#define PS_DEPTH_SOURCE_FILE            "depth.frag"
#define VS_GBUFFER_SOURCE_FILE          "gbuffer.vert"
#define PS_GBUFFER_SOURCE_FILE          "gbuffer.frag"
#define VS_SSAO_SOURCE_FILE             "fullscreen.vert"
#define PS_SSAO_SOURCE_FILE             "ssao.frag"
#define VS_DEFFERED_VCT_SOURCE_FILE     "fullscreen.vert"
#define PS_DEFFERED_VCT_SOURCE_FILE     "vct_deferred.frag"
#define VS_FXAA_SOURCE_FILE             "fullscreen.vert"
#define PS_FXAA_SOURCE_FILE             "vct_deferred.frag"
#define VS_VOXEL_SOURCE_FILE            "voxel/voxel.vert"
#define GS_VOXEL_SOURCE_FILE            "voxel/voxel.geom"
#define PS_VOXEL_SOURCE_FILE            "voxel/voxel.frag"
#define VS_VOXEL_VISIUALIZE_SOURCE_FILE "voxel/visualization.vert"
#define PS_VOXEL_VISIUALIZE_SOURCE_FILE "voxel/visualization.frag"
#define CS_VOXEL_POST_SOURCE_FILE       "voxel/post.comp"

PipelineStateManager::~PipelineStateManager()
{
}

bool PipelineStateManager::RegisterPipelineState( PipelineState& pipelineState )
{
#if USING( ENABLE_ASSERT )
    switch ( pipelineState.pipelineType ) {
        case PIPELINE_TYPE::COMPUTE:
            ASSERT( !pipelineState.computeShaderName.empty() && pipelineState.vertexShaderName.empty() && pipelineState.geometryShaderName.empty() && pipelineState.pixelShaderName.empty() );
            break;
        case PIPELINE_TYPE::GRAPHIC:
            ASSERT( pipelineState.computeShaderName.empty() && !pipelineState.vertexShaderName.empty() && !pipelineState.pixelShaderName.empty() );
            break;
        default:
            UNREACHABLE();
            break;
    }
#endif

    PipelineState* pPipelineState = &pipelineState;
    if ( InitializePipelineState( &pPipelineState ) ) {
        m_pipelineStates.emplace( pipelineState.pipelineStateName, pPipelineState );
        LOG_DEBUG( "PipelineState %s registered.", pipelineState.pipelineStateName.c_str() );
        return true;
    }

    LOG_ERROR( "PipelineState %s registered failed.", pipelineState.pipelineStateName.c_str() );
    return false;
}

void PipelineStateManager::UnregisterPipelineState(
    PipelineState& pipelineState )
{
    const auto& it = m_pipelineStates.find( pipelineState.pipelineStateName );
    if ( it != m_pipelineStates.end() ) {
        DestroyPipelineState( *it->second );
    }
    m_pipelineStates.erase( it );
}

void PipelineStateManager::Clear()
{
    for ( auto it = m_pipelineStates.begin(); it != m_pipelineStates.end();
          it++ ) {
        if ( it != m_pipelineStates.end() ) {
            DestroyPipelineState( *it->second );
        }
    }

    m_pipelineStates.clear();
    ASSERT( m_pipelineStates.empty() );

    LOG_DEBUG( "Pipeline State Manager Clear has been called. " );
}

const std::shared_ptr<PipelineState> PipelineStateManager::GetPipelineState(
    std::string name ) const
{
    const auto& it = m_pipelineStates.find( name );
    if ( it != m_pipelineStates.end() ) {
        return it->second;
    }
    else {
        ASSERT( !m_pipelineStates.empty() );
        LOG_ERROR( "Pipeline State %s not found", name.c_str() );
        return m_pipelineStates.begin()->second;
    }
}

bool PipelineStateManager::Init()
{
    struct PipelineStateEx : PipelineState {
        explicit PipelineStateEx( const char* name )
        {
            pipelineStateName = name;
            pipelineType = PIPELINE_TYPE::GRAPHIC;
            cullFaceMode = CULL_FACE_MODE::BACK;
            depthTestMode = DEPTH_TEST_MODE::LESS_EQUAL;
            stencilTestMode = STENCIL_TEST_MODE::NONE;
        }
    };

    {
        PipelineStateEx pipelineState{ "LINE3D" };
        pipelineState.vertexShaderName = VS_HUD_LINE3D_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_HUD_LINE3D_SOURCE_FILE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::BACK;
        pipelineState.depthTestMode = DEPTH_TEST_MODE::NONE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "IMAGE2D" };
        pipelineState.vertexShaderName = VS_HUD_IMAGE_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_HUD_IMAGE_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "DEBUG_TEXTURE" };
        pipelineState.vertexShaderName = VS_HUD_TEXTURE_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_HUD_TEXTURE_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "SHADOW" };
        pipelineState.vertexShaderName = VS_DEPTH_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_DEPTH_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "GBUFFER" };
        pipelineState.vertexShaderName = VS_GBUFFER_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_GBUFFER_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "SSAO" };
        pipelineState.vertexShaderName = VS_SSAO_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_SSAO_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "VCT" };
        pipelineState.vertexShaderName = VS_DEFFERED_VCT_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_DEFFERED_VCT_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "FXAA" };
        pipelineState.vertexShaderName = VS_FXAA_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_FXAA_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "VOXEL" };
        pipelineState.vertexShaderName = VS_VOXEL_SOURCE_FILE;
        pipelineState.geometryShaderName = GS_VOXEL_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_VOXEL_SOURCE_FILE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::NONE;
        pipelineState.depthTestMode = DEPTH_TEST_MODE::NONE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "VOXEL_VIS" };
        pipelineState.vertexShaderName = VS_VOXEL_VISIUALIZE_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_VOXEL_VISIUALIZE_SOURCE_FILE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::NONE;
        pipelineState.depthTestMode = DEPTH_TEST_MODE::LESS_EQUAL;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "VOXEL_POST" };
        pipelineState.pipelineType = PIPELINE_TYPE::COMPUTE;
        pipelineState.computeShaderName = CS_VOXEL_POST_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    LOG_OK( "Pipeline State Manager Initialized. [%zu]", m_pipelineStates.size() );
    return true;
}

void PipelineStateManager::Deinit()
{
    Clear();
}
