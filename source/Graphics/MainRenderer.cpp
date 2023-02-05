#pragma once
#include "MainRenderer.h"

#include "Base/Asserts.h"

#include "Core/com_dvars.h"
#include "Core/com_misc.h"
#include "Core/editor.h"
#include "Core/geometry.h"
#include "Core/FileManager.h"
#include "Core/WindowManager.h"
#include "r_defines.h"
#include "r_editor.h"
#include "r_passes.h"
#include "r_rendertarget.h"
#include "r_sun_shadow.h"

#include "GraphicsManager.hpp"

static std::vector<std::shared_ptr<MeshData>> g_meshdata;
static std::vector<std::shared_ptr<MaterialData>> g_materialdata;

namespace vct {

static std::shared_ptr<MeshData> CreateMeshData( const MeshComponent& mesh )
{
    MeshData* ret = new MeshData;

    MeshData& outMesh = *ret;
    const bool hasNormals = !mesh.normals.empty();
    const bool hasUVs = !mesh.uvs.empty();
    const bool hasTangent = !mesh.tangents.empty();
    const bool hasBitangent = !mesh.bitangents.empty();

    glGenVertexArrays( 1, &outMesh.vao );
    glGenBuffers( 2 + hasNormals + hasUVs + hasTangent + hasBitangent, &outMesh.ebo );
    glBindVertexArray( outMesh.vao );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, outMesh.ebo );
    gl::BindToSlot( outMesh.vbos[0], 0, 3 );
    gl::NamedBufferStorage( outMesh.vbos[0], mesh.positions );
    if ( hasNormals ) {
        gl::BindToSlot( outMesh.vbos[1], 1, 3 );
        gl::NamedBufferStorage( outMesh.vbos[1], mesh.normals );
    }
    if ( hasUVs ) {
        gl::BindToSlot( outMesh.vbos[2], 2, 2 );
        gl::NamedBufferStorage( outMesh.vbos[2], mesh.uvs );
    }
    if ( hasTangent ) {
        gl::BindToSlot( outMesh.vbos[3], 3, 3 );
        gl::NamedBufferStorage( outMesh.vbos[3], mesh.tangents );
        gl::BindToSlot( outMesh.vbos[4], 4, 3 );
        gl::NamedBufferStorage( outMesh.vbos[4], mesh.bitangents );
    }

    gl::NamedBufferStorage( outMesh.ebo, mesh.indices );
    outMesh.count = static_cast<uint32_t>( mesh.indices.size() );

    glBindVertexArray( 0 );
    return std::shared_ptr<MeshData>( ret );
}

void MainRenderer::createGpuResources()
{
    R_Create_Pass_Resources();

    R_Alloc_Cbuffers();
    R_CreateEditorResource();
    R_CreateRT();

    Scene& scene = Com_GetScene();

    m_box = CreateMeshData( geometry::MakeBox() );

    // create box quad
    R_CreateQuad();

    const int voxelSize = Dvar_GetInt( r_voxelSize );

    /// create voxel image
    {
        Texture3DCreateInfo info;
        info.wrapS = info.wrapT = info.wrapR = GL_CLAMP_TO_BORDER;
        info.size = voxelSize;
        info.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        info.magFilter = GL_NEAREST;
        info.mipLevel = log_two( voxelSize );
        info.format = GL_RGBA16F;

        m_albedoVoxel.create3DEmpty( info );
        m_normalVoxel.create3DEmpty( info );
    }

    // create mesh
    for ( const auto& mesh : scene.meshes ) {
        g_meshdata.emplace_back( CreateMeshData( *mesh.get() ) );
        mesh->gpuResource = g_meshdata.back().get();
    }

    // create material
    ASSERT( scene.materials.size() < array_length( g_constantCache.cache.AlbedoMaps ) );

    for ( int idx = 0; idx < scene.materials.size(); ++idx ) {
        const auto& mat = scene.materials.at( idx );

        std::shared_ptr<MaterialData> matData( new MaterialData() );
        if ( !mat->albedoTexture.empty() ) {
            matData->albedoColor = vec4( 0 );
            matData->albedoMap.create2DImageFromFile( mat->albedoTexture.c_str() );
            g_constantCache.cache.AlbedoMaps[idx].data = gl::MakeTextureResident( matData->albedoMap.GetHandle() );
        }
        else {
            matData->albedoColor = vec4( mat->albedo, 1.0f );
        }

        if ( !mat->metallicRoughnessTexture.empty() ) {
            matData->materialMap.create2DImageFromFile( mat->metallicRoughnessTexture.c_str() );
            g_constantCache.cache.PbrMaps[idx].data = gl::MakeTextureResident( matData->materialMap.GetHandle() );
        }
        else {
            matData->metallic = mat->metallic;
            matData->roughness = mat->roughness;
        }

        if ( !mat->normalTexture.empty() ) {
            matData->normalMap.create2DImageFromFile( mat->normalTexture.c_str() );
            g_constantCache.cache.NormalMaps[idx].data = gl::MakeTextureResident( matData->normalMap.GetHandle() );
            LOG_INFO( "material has bump %s", mat->normalTexture.c_str() );
        }

        matData->textureMapIdx = idx;

        matData->reflectPower = mat->reflectPower;

        g_materialdata.emplace_back( matData );
        mat->gpuResource = g_materialdata.back().get();
    }

    g_constantCache.cache.ShadowMap = gl::MakeTextureResident( g_shadowRT.GetDepthTexture().GetHandle() );
    g_constantCache.cache.GbufferDepthMap = gl::MakeTextureResident( g_gbufferRT.GetDepthTexture().GetHandle() );
    g_constantCache.cache.GbufferPositionMetallicMap = gl::MakeTextureResident( g_gbufferRT.GetColorAttachment( 0 ).GetHandle() );
    g_constantCache.cache.GbufferNormalRoughnessMap = gl::MakeTextureResident( g_gbufferRT.GetColorAttachment( 1 ).GetHandle() );
    g_constantCache.cache.GbufferAlbedoMap = gl::MakeTextureResident( g_gbufferRT.GetColorAttachment( 2 ).GetHandle() );
    g_constantCache.cache.VoxelAlbedoMap = gl::MakeTextureResident( m_albedoVoxel.GetHandle() );
    g_constantCache.cache.VoxelNormalMap = gl::MakeTextureResident( m_normalVoxel.GetHandle() );
    g_constantCache.cache.SSAOMap = gl::MakeTextureResident( g_ssaoRT.GetColorAttachment().GetHandle() );
    g_constantCache.cache.FinalImage = gl::MakeTextureResident( g_finalImageRT.GetColorAttachment().GetHandle() );
    g_constantCache.cache.FXAA = gl::MakeTextureResident( g_fxaaRT.GetColorAttachment().GetHandle() );

    for ( int idx = 0; idx < 1; ++idx ) {
        std::string path = g_fileMgr->BuildAbsPath( "pointlight.png", "data/images" );
        m_lightIcons[idx].create2DImageFromFile( path.c_str() );
        g_constantCache.cache.LightIconTextures[idx].data = gl::MakeTextureResident( m_lightIcons[idx].GetHandle() );
    }

    g_constantCache.Update();
}

// @TODO: make another pass
void MainRenderer::visualizeVoxels()
{
    auto PSO = g_pPipelineStateManager->GetPipelineState( "VOXEL_VIS" );
    g_gfxMgr->SetPipelineState( PSO );
    glBindVertexArray( m_box->vao );
    const int size = Dvar_GetInt( r_voxelSize );
    glDrawElementsInstanced( GL_TRIANGLES, m_box->count, GL_UNSIGNED_INT, 0, size * size * size );
}

struct MaterialCache {
    vec4 albedo_color;  // if it doesn't have albedo color, then it's alpha is 0.0f
    float metallic = 0.0f;
    float roughness = 0.0f;
    float has_metallic_roughness_texture = 0.0f;
    float has_normal_texture = 0.0f;
    float reflect = 0.0f;

    MaterialCache& operator=( const MaterialData& mat )
    {
        albedo_color = mat.albedoColor;
        roughness = mat.roughness;
        metallic = mat.metallic;
        reflect = mat.reflectPower;
        has_metallic_roughness_texture = mat.materialMap.GetHandle() == 0 ? 0.0f : 1.0f;
        has_normal_texture = mat.normalMap.GetHandle() == 0 ? 0.0f : 1.0f;

        return *this;
    }
};

void MainRenderer::renderToVoxelTexture()
{
    const int voxelSize = Dvar_GetInt( r_voxelSize );

    // @TODO: move to PSO
    glDisable( GL_BLEND );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glViewport( 0, 0, voxelSize, voxelSize );

    m_albedoVoxel.bindImageTexture( IMAGE_VOXEL_ALBEDO_SLOT );
    m_normalVoxel.bindImageTexture( IMAGE_VOXEL_NORMAL_SLOT );

    auto PSO = g_pPipelineStateManager->GetPipelineState( "VOXEL" );
    g_gfxMgr->SetPipelineState( PSO );

    Frame dummy;
    g_gfxMgr->DrawBatch( dummy );

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    // @TODO: make another pass
    // post process
    {
        auto PSO = g_pPipelineStateManager->GetPipelineState( "VOXEL_POST" );
        g_gfxMgr->SetPipelineState( PSO );

        constexpr GLuint workGroupX = 512;
        constexpr GLuint workGroupY = 512;
        const GLuint workGroupZ =
            ( voxelSize * voxelSize * voxelSize ) /
            ( workGroupX * workGroupY );

        glDispatchCompute( workGroupX, workGroupY, workGroupZ );
        glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

        m_albedoVoxel.bind();
        m_albedoVoxel.genMipMap();
        m_normalVoxel.bind();
        m_normalVoxel.genMipMap();
    }
}

void MainRenderer::renderFrameBufferTextures( const ivec2& extent )
{
    auto PSO = g_pPipelineStateManager->GetPipelineState( "DEBUG_TEXTURE" );
    g_gfxMgr->SetPipelineState( PSO );

    glViewport( 0, 0, extent.x, extent.y );

    R_DrawQuad();
}

void MainRenderer::render()
{
    Scene& scene = Com_GetScene();

    // clear window
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    R_ShadowPass();

    if ( scene.dirty || Dvar_GetBool( r_forceVXGI ) ) {
        m_albedoVoxel.clear();
        m_normalVoxel.clear();
        renderToVoxelTexture();
    }

    ivec2 extent = g_wndMgr->FrameSize();
    if ( extent.x * extent.y > 0 ) {
        // skip rendering if minimized
        glViewport( 0, 0, extent.x, extent.y );

        R_Gbuffer_Pass();
        R_SSAO_Pass();

        const int mode = Dvar_GetInt( r_debugTexture );

        switch ( mode ) {
            case DrawTexture::TEXTURE_VOXEL_ALBEDO:
            case DrawTexture::TEXTURE_VOXEL_NORMAL:
                visualizeVoxels();
                break;
            default: {
                R_Deferred_VCT_Pass();
                R_FXAA_Pass();

                renderFrameBufferTextures( extent );
            } break;
        }

        R_DrawEditor();
    }
}

void MainRenderer::destroyGpuResources()
{
    R_DestroyRT();

    R_DestroyEditorResource();
    R_Destroy_Cbuffers();

    R_Destroy_Pass_Resources();
}

}  // namespace vct
