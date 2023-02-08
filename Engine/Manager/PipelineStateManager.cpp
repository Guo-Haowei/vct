#include "PipelineStateManager.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Interface/IAssetLoader.hpp"
#include "Manager/BaseApplication.hpp"

#define VS_SIMPLE_SOURCE_FILE           "simple.vert"
#define PS_SIMPLE_SOURCE_FILE           "simple.frag"
#define VS_HUD_LINE3D_SOURCE_FILE       "hud/line3d.vert"
#define PS_HUD_LINE3D_SOURCE_FILE       "hud/line3d.frag"
#define VS_HUD_IMAGE_SOURCE_FILE        "hud/image.vert"
#define PS_HUD_IMAGE_SOURCE_FILE        "hud/image.frag"
#define VS_SHADOWMAP_SOURCE_FILE        "shadowmap.vert"
#define PS_SHADOWMAP_SOURCE_FILE        "shadowmap.frag"
#define VS_FORWARD_SOURCE_FILE          "forward.vert"
#define PS_FORWARD_SOURCE_FILE          "forward.pixel"
#define VS_DEFFERED_VCT_SOURCE_FILE     "fullscreen.vert"
#define PS_DEFFERED_VCT_SOURCE_FILE     "vct_deferred.frag"
#define VS_VOXEL_SOURCE_FILE            "voxel/voxel.vert"
#define GS_VOXEL_SOURCE_FILE            "voxel/voxel.geom"
#define PS_VOXEL_SOURCE_FILE            "voxel/voxel.frag"
#define VS_VOXEL_VISIUALIZE_SOURCE_FILE "voxel/visualization.vert"
#define PS_VOXEL_VISIUALIZE_SOURCE_FILE "voxel/visualization.frag"
#define CS_VOXEL_POST_SOURCE_FILE       "voxel/post.comp"
#define VS_HUD_OVERLAY_SOURCE_FILE      "hud/overlay.vert"
#define PS_HUD_OVERLAY_SOURCE_FILE      "hud/overlay.frag"

PipelineStateManager::~PipelineStateManager()
{
    ASSERT( m_pipelineStates.empty() );
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
        LOG_OK( "PipelineState [%s].", pipelineState.pipelineStateName.c_str() );
        return true;
    }

    LOG_ERROR( "Failed to register PipelineState [%s].", pipelineState.pipelineStateName.c_str() );
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

const std::shared_ptr<PipelineState> PipelineStateManager::GetPipelineState( const std::string& name ) const
{
    const auto& it = m_pipelineStates.find( name );
    if ( it != m_pipelineStates.end() ) {
        return it->second;
    }
    else {
        ASSERT( !m_pipelineStates.empty() );
        LOG_FATAL( "Pipeline State %s not found", name.c_str() );
        return m_pipelineStates.begin()->second;
    }
}

bool PipelineStateManager::Initialize()
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

    BaseApplication* pApp = dynamic_cast<BaseApplication*>( m_pApp );

    if ( pApp->GetGfxBackend() == GfxBackend::D3d11 ) {
        PipelineStateEx pipelineState{ "SIMPLE" };
        pipelineState.vertexShaderName = VS_SIMPLE_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_SIMPLE_SOURCE_FILE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "LINE3D" };
        pipelineState.vertexShaderName = VS_HUD_LINE3D_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_HUD_LINE3D_SOURCE_FILE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::BACK;
        pipelineState.depthTestMode = DEPTH_TEST_MODE::NONE;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "SHADOW" };
        pipelineState.vertexShaderName = VS_SHADOWMAP_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_SHADOWMAP_SOURCE_FILE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::FRONT;
        RegisterPipelineState( pipelineState );
    }

    {
        PipelineStateEx pipelineState{ "FORWARD" };
        pipelineState.vertexShaderName = VS_FORWARD_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_FORWARD_SOURCE_FILE;
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

    {
        PipelineStateEx pipelineState{ "OVERLAY" };
        pipelineState.vertexShaderName = VS_HUD_OVERLAY_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_HUD_OVERLAY_SOURCE_FILE;
        pipelineState.depthTestMode = DEPTH_TEST_MODE::ALWAYS;
        RegisterPipelineState( pipelineState );
    }

    LOG_OK( "Pipeline State Manager Initialized. [%zu]", m_pipelineStates.size() );
    return true;
}

void PipelineStateManager::Finalize()
{
    Clear();
}
