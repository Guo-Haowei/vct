#pragma once
#include "MainRenderer.h"

#include "common/com_dvars.h"
#include "common/com_filesystem.h"
#include "common/com_misc.h"
#include "common/editor.h"
#include "common/geometry.h"
#include "common/main_window.h"
#include "r_defines.h"
#include "r_editor.h"
#include "r_shader.h"
#include "r_sun_shadow.h"
#include "universal/core_assert.h"
#include "universal/dvar_api.h"

static std::vector<std::shared_ptr<MeshData>> g_meshdata;
static std::vector<std::shared_ptr<MaterialData>> g_materialdata;

DepthRenderTarget g_shadowBuffer;

namespace vct {

static std::shared_ptr<MeshData> CreateMeshData( const MeshComponent& mesh )
{
    MeshData* ret = new MeshData;

    MeshData& outMesh       = *ret;
    const bool hasNormals   = !mesh.normals.empty();
    const bool hasUVs       = !mesh.uvs.empty();
    const bool hasTangent   = !mesh.tangents.empty();
    const bool hasBitangent = !mesh.bitangents.empty();

    glGenVertexArrays( 1, &outMesh.vao );
    glGenBuffers( 2 + hasNormals + hasUVs + hasTangent + hasBitangent, &outMesh.ebo );
    glBindVertexArray( outMesh.vao );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, outMesh.ebo );
    gl::BindToSlot( outMesh.vbos[0], 0, 3 );
    gl::NamedBufferStorage( outMesh.vbos[0], mesh.positions );
    if ( hasNormals )
    {
        gl::BindToSlot( outMesh.vbos[1], 1, 3 );
        gl::NamedBufferStorage( outMesh.vbos[1], mesh.normals );
    }
    if ( hasUVs )
    {
        gl::BindToSlot( outMesh.vbos[2], 2, 2 );
        gl::NamedBufferStorage( outMesh.vbos[2], mesh.uvs );
    }
    if ( hasTangent )
    {
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
    R_CreateShaderPrograms();

    R_Alloc_Cbuffers();
    R_CreateEditorResource();

    Scene& scene = Com_GetScene();

    // create shader
    m_gbufferProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "gbuffer" ) ) );
    m_vctProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "fullscreen", "vct_deferred" ) ) );
    m_debugTextureProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "fullscreen", "debug/texture" ) ) );
    m_voxelProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSGSPS( "voxel/voxelization" ) ) );
    m_visualizeProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "voxel/visualization" ) ) );
    m_voxelPostProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::CS( "voxel/post" ) ) );

    // frame buffers
    createFrameBuffers();

    m_box = CreateMeshData( geometry::MakeBox() );

    // create box quad
    {
        // clang-format off
        float points[] = { -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, };
        // clang-format on
        glGenVertexArrays( 1, &m_quad.vao );
        glGenBuffers( 1, m_quad.vbos );
        glBindVertexArray( m_quad.vao );

        glBindBuffer( GL_ARRAY_BUFFER, m_quad.vbos[0] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( points ), points, GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( float ), 0 );
        glEnableVertexAttribArray( 0 );
    }

    /// create voxel image
    {
        Texture3DCreateInfo info;
        info.wrapS = info.wrapT = info.wrapR = GL_CLAMP_TO_BORDER;
        info.size                            = VOXEL_TEXTURE_SIZE;
        info.minFilter                       = GL_LINEAR_MIPMAP_LINEAR;
        info.magFilter                       = GL_NEAREST;
        info.mipLevel                        = VOXEL_TEXTURE_MIP_LEVEL;
        info.format                          = GL_RGBA16F;

        m_albedoVoxel.create3DEmpty( info );
        m_normalVoxel.create3DEmpty( info );
    }

    // create mesh
    for ( const auto& mesh : scene.meshes )
    {
        g_meshdata.emplace_back( CreateMeshData( *mesh.get() ) );
        mesh->gpuResource = g_meshdata.back().get();
    }

    // create material
    core_assert( scene.materials.size() < array_length( g_constantCache.cache.AlbedoMaps ) );

    for ( int idx = 0; idx < scene.materials.size(); ++idx )
    {
        const auto& mat = scene.materials.at( idx );

        std::shared_ptr<MaterialData> matData( new MaterialData() );
        if ( !mat->albedoTexture.empty() )
        {
            matData->albedoColor = vec4( 0 );
            matData->albedoMap.create2DImageFromFile( mat->albedoTexture.c_str() );
            g_constantCache.cache.AlbedoMaps[idx].data = gl::MakeTextureResident( matData->albedoMap.GetHandle() );
        }
        else
        {
            matData->albedoColor = vec4( mat->albedo, 1.0f );
        }

        if ( !mat->metallicRoughnessTexture.empty() )
        {
            matData->materialMap.create2DImageFromFile( mat->metallicRoughnessTexture.c_str() );
            g_constantCache.cache.PbrMaps[idx].data = gl::MakeTextureResident( matData->materialMap.GetHandle() );
        }
        else
        {
            matData->metallic  = mat->metallic;
            matData->roughness = mat->roughness;
        }

        if ( !mat->normalTexture.empty() )
        {
            matData->normalMap.create2DImageFromFile( mat->normalTexture.c_str() );
            g_constantCache.cache.NormalMaps[idx].data = gl::MakeTextureResident( matData->normalMap.GetHandle() );
        }

        matData->textureMapIdx = idx;
        g_materialdata.emplace_back( matData );
        mat->gpuResource = g_materialdata.back().get();
    }

    g_constantCache.cache.ShadowMap                  = gl::MakeTextureResident( g_shadowBuffer.getDepthTexture().GetHandle() );
    g_constantCache.cache.GbufferDepthMap            = gl::MakeTextureResident( m_gbuffer.getDepthTexture().GetHandle() );
    g_constantCache.cache.GbufferPositionMetallicMap = gl::MakeTextureResident( m_gbuffer.getColorAttachment( 0 ).GetHandle() );
    g_constantCache.cache.GbufferNormalRoughnessMap  = gl::MakeTextureResident( m_gbuffer.getColorAttachment( 1 ).GetHandle() );
    g_constantCache.cache.GbufferAlbedoMap           = gl::MakeTextureResident( m_gbuffer.getColorAttachment( 2 ).GetHandle() );
    g_constantCache.cache.VoxelAlbedoMap             = gl::MakeTextureResident( m_albedoVoxel.GetHandle() );
    g_constantCache.cache.VoxelNormalMap             = gl::MakeTextureResident( m_normalVoxel.GetHandle() );

    char buffer[kMaxOSPath];
    for ( int idx = 0; idx < 1; ++idx )
    {
        Com_FsBuildPath( buffer, kMaxOSPath, "pointlight.png", "data/images" );
        m_lightIcons[idx].create2DImageFromFile( buffer );
        g_constantCache.cache.LightIconTextures[idx].data = gl::MakeTextureResident( m_lightIcons[idx].GetHandle() );
    }

    g_constantCache.Update();
}

void MainRenderer::visualizeVoxels()
{
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );

    GlslProgram& program = m_visualizeProgram;

    glBindVertexArray( m_box->vao );
    program.Use();

    // GpuTexture& voxelTexture = isAlbedo ? m_albedoVoxel : m_normalVoxel;

    // glBindImageTexture( 0, voxelTexture.GetHandle(), mipLevel, GL_TRUE, 1,
    //                     GL_READ_ONLY, voxelTexture.getFormat() );

    constexpr int size = VOXEL_TEXTURE_SIZE;
    glDrawElementsInstanced( GL_TRIANGLES, m_box->count, GL_UNSIGNED_INT, 0, size * size * size );

    program.Stop();
}

void FillMaterialCB( const MaterialData* mat, MaterialCB& cb )
{
    cb.AlbedoColor   = mat->albedoColor;
    cb.Metallic      = mat->metallic;
    cb.Roughness     = mat->roughness;
    cb.HasAlbedoMap  = mat->albedoMap.GetHandle() != 0;
    cb.HasNormalMap  = mat->materialMap.GetHandle() != 0;
    cb.HasPbrMap     = mat->materialMap.GetHandle() != 0;
    cb.TextureMapIdx = mat->textureMapIdx;
}

struct MaterialCache {
    vec4 albedo_color;  // if it doesn't have albedo color, then it's alpha is 0.0f
    float metallic                       = 0.0f;
    float roughness                      = 0.0f;
    float has_metallic_roughness_texture = 0.0f;
    float has_normal_texture             = 0.0f;

    MaterialCache& operator=( const MaterialData& mat )
    {
        albedo_color                   = mat.albedoColor;
        roughness                      = mat.roughness;
        metallic                       = mat.metallic;
        has_metallic_roughness_texture = mat.materialMap.GetHandle() == 0 ? 0.0f : 1.0f;
        has_normal_texture             = mat.normalMap.GetHandle() == 0 ? 0.0f : 1.0f;

        return *this;
    }
};

void MainRenderer::gbufferPass()
{
    Scene& scene         = Com_GetScene();
    GlslProgram& program = m_gbufferProgram;

    m_gbuffer.bind();
    program.Use();

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Frustum frustum( scene.camera.ProjView() );
    for ( const GeometryNode& node : scene.geometryNodes )
    {
        g_perBatchCache.cache.Model = node.transform;
        g_perBatchCache.cache.PVM   = g_perFrameCache.cache.PV * node.transform;
        g_perBatchCache.Update();

        for ( const Geometry& geom : node.geometries )
        {
            if ( !geom.visible )
            {
                continue;
            }
            if ( !frustum.Intersect( geom.boundingBox ) )
            {
                continue;
            }

            const MeshData* drawData    = reinterpret_cast<MeshData*>( geom.pMesh->gpuResource );
            const MaterialData* matData = reinterpret_cast<MaterialData*>( geom.pMaterial->gpuResource );

            FillMaterialCB( matData, g_materialCache.cache );
            g_materialCache.Update();

            glBindVertexArray( drawData->vao );
            glDrawElements( GL_TRIANGLES, drawData->count, GL_UNSIGNED_INT, 0 );
        }
    }

    m_gbufferProgram.Stop();
    m_gbuffer.unbind();
}

void MainRenderer::renderToVoxelTexture()
{
    Scene& scene = Com_GetScene();

    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glViewport( 0, 0, VOXEL_TEXTURE_SIZE, VOXEL_TEXTURE_SIZE );

    m_albedoVoxel.bindImageTexture( IMAGE_VOXEL_ALBEDO_SLOT );
    m_normalVoxel.bindImageTexture( IMAGE_VOXEL_NORMAL_SLOT );
    m_voxelProgram.Use();

    for ( const GeometryNode& node : scene.geometryNodes )
    {
        g_perBatchCache.cache.Model = node.transform;
        g_perBatchCache.cache.PVM   = g_perFrameCache.cache.PV * node.transform;
        g_perBatchCache.Update();

        for ( const Geometry& geom : node.geometries )
        {
            if ( !geom.visible )
            {
                continue;
            }

            const MeshData* drawData    = reinterpret_cast<const MeshData*>( geom.pMesh->gpuResource );
            const MaterialData* matData = reinterpret_cast<const MaterialData*>( geom.pMaterial->gpuResource );

            FillMaterialCB( matData, g_materialCache.cache );
            g_materialCache.Update();

            glBindVertexArray( drawData->vao );
            glDrawElements( GL_TRIANGLES, drawData->count, GL_UNSIGNED_INT, 0 );
        }
    }

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    // post process
    m_voxelPostProgram.Use();

    constexpr GLuint workGroupX = 512;
    constexpr GLuint workGroupY = 512;
    constexpr GLuint workGroupZ =
        ( VOXEL_TEXTURE_SIZE * VOXEL_TEXTURE_SIZE * VOXEL_TEXTURE_SIZE ) /
        ( workGroupX * workGroupY );

    glDispatchCompute( workGroupX, workGroupY, workGroupZ );
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

    m_albedoVoxel.bind();
    m_albedoVoxel.genMipMap();
    m_normalVoxel.bind();
    m_normalVoxel.genMipMap();
}

void MainRenderer::vctPass()
{
    GlslProgram& program = m_vctProgram;

    program.Use();

    glBindVertexArray( m_quad.vao );
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    program.Stop();
}

void MainRenderer::renderFrameBufferTextures( const ivec2& extent )
{
    GlslProgram& program = m_debugTextureProgram;

    program.Use();

    glDisable( GL_DEPTH_TEST );

    constexpr float scale = 0.13f;
    int width             = static_cast<int>( scale * extent.x );
    int height            = static_cast<int>( scale * extent.y );

    glBindVertexArray( m_quad.vao );
    glViewport( 0, 0, extent.x, extent.y );

    enum {
        DrawVoxelAlbedo = 0,
        DrawVoxelNormal = 1,
        DrawAlbedo      = 2,
        DrawNormal      = 3,
    };

    glDrawArrays( GL_TRIANGLES, 0, 6 );

    program.Stop();
}

void MainRenderer::render()
{
    Scene& scene = Com_GetScene();

    g_perFrameCache.Update();

    R_ShadowPass();

    if ( scene.dirty || Dvar_GetBool( r_forceVXGI ) )
    {
        m_albedoVoxel.clear();
        m_normalVoxel.clear();
        renderToVoxelTexture();
    }

    ivec2 extent = MainWindow::FrameSize();
    if ( extent.x * extent.y > 0 )
    {
        // skip rendering if minimized
        glViewport( 0, 0, extent.x, extent.y );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glEnable( GL_DEPTH_TEST );
        glEnable( GL_CULL_FACE );

        gbufferPass();

        const int mode = Dvar_GetInt( r_debugTexture );
        if ( mode == DrawTexture::TEXTURE_FINAL_IMAGE )
        {
            vctPass();
        }
        else if ( mode <= DrawTexture::TEXTURE_VOXEL_COUNT )
        {
            visualizeVoxels();
        }
        else
        {
            renderFrameBufferTextures( extent );
        }

        R_DrawEditor();
    }
}

void MainRenderer::createFrameBuffers()
{
    const ivec2 extent = MainWindow::FrameSize();

    const int res = Dvar_GetInt( r_shadowRes );
    g_shadowBuffer.create( NUM_CASCADES * res, res );
    core_assert( is_power_of_two( res ) );

    m_gbuffer.create( extent.x, extent.y );
}

void MainRenderer::destroyGpuResources()
{
    // gpu resource
    m_voxelProgram.Destroy();
    m_visualizeProgram.Destroy();
    m_voxelPostProgram.Destroy();
    m_debugTextureProgram.Destroy();

    // render targets
    g_shadowBuffer.destroy();
    m_gbuffer.destroy();

    R_DestroyEditorResource();
    R_Destroy_Cbuffers();

    R_DestroyShaderPrograms();
}

}  // namespace vct
