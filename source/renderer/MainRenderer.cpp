#pragma once
#include "MainRenderer.h"

#include <iostream>
#include <unordered_map>

#include "common/Globals.h"
#include "common/com_system.h"
#include "common/editor.h"
#include "common/geometry.h"
#include "common/main_window.h"
#include "scene/Scene.h"
#include "universal/core_assert.h"

namespace vct {

static std::unordered_map<const MeshComponent*, MeshData> g_meshLUT;
static std::unordered_map<const Material*, MaterialData> g_matLUT;

constexpr int UNIFORM_BUFFER_VS_PER_FRAME_SLOT = 0;
constexpr int UNIFORM_BUFFER_FS_PER_FRAME_SLOT = 1;
constexpr int UNIFORM_BUFFER_MATERIAL_SLOT     = 2;
constexpr int UNIFORM_BUFFER_CONSTANT_SLOT     = 3;

constexpr int IMAGE_VOXEL_ALBEDO_SLOT   = 0;
constexpr int IMAGE_VOXEL_NORMAL_SLOT   = 1;
constexpr int TEXTURE_VOXEL_ALBEDO_SLOT = 2;
constexpr int TEXTURE_VOXEL_NORMAL_SLOT = 3;

constexpr int ALBEDO_MAP_SLOT         = 4;
constexpr int NORMAL_MAP_SLOT         = 6;
constexpr int METALLIC_ROUGHNESS_SLOT = 7;

constexpr int TEXTURE_SHADOW_MAP_SLOT                = 3;
constexpr int TEXTURE_GBUFFER_DEPTH_SLOT             = 9;
constexpr int TEXTURE_GBUFFER_ALBEDO_SLOT            = 10;
constexpr int TEXTURE_GBUFFER_NORMAL_ROUGHNESS_SLOT  = 11;
constexpr int TEXTURE_GBUFFER_POSITION_METALLIC_SLOT = 12;
constexpr int TEXTURE_GBUFFER_AO_SLOT                = 13;

// TODO: remove this
#ifndef DATA_DIR
#define DATA_DIR ""
#endif

static MeshData CreateMeshData( const MeshComponent& mesh )
{
    MeshData outMesh;
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
    return outMesh;
}

void MainRenderer::createGpuResources()
{
    R_Alloc_Cbuffers();

    Scene scene = Com_GetScene();

    vec3 center     = scene.boundingBox.Center();
    vec3 size       = scene.boundingBox.Size();
    float worldSize = std::max( size.x, std::max( size.y, size.z ) );
    float texelSize = 1.0f / static_cast<float>( VOXEL_TEXTURE_SIZE );
    float voxelSize = worldSize * texelSize;

    // create uniform buffer
    g_perframeCache.cache.LightPV    = lightSpaceMatrix( scene.light.position, scene.shadowBox );
    g_perframeCache.cache.LightPos   = scene.light.position;
    g_perframeCache.cache.LightColor = scene.light.color;
    g_perframeCache.cache.CamPos     = scene.camera.position;
    g_perframeCache.Update();

    m_constantBuffer.CreateAndBind( UNIFORM_BUFFER_CONSTANT_SLOT );
    m_constantBuffer.cache.world_center    = center;
    m_constantBuffer.cache.world_size_half = 0.5f * worldSize;
    m_constantBuffer.cache.texel_size      = texelSize;
    m_constantBuffer.cache.voxel_size      = voxelSize;
    m_constantBuffer.Update();

    m_fsMaterialBuffer.CreateAndBind( UNIFORM_BUFFER_MATERIAL_SLOT );

    // create shader
    m_gbufferProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "gbuffer" ) ) );
    {
        m_gbufferProgram.use();
        m_gbufferProgram.setUniform( m_gbufferProgram.getUniformLocation( "u_albedo_map" ), ALBEDO_MAP_SLOT );
        m_gbufferProgram.setUniform( m_gbufferProgram.getUniformLocation( "u_normal_map" ), NORMAL_MAP_SLOT );
        m_gbufferProgram.setUniform( m_gbufferProgram.getUniformLocation( "u_metallic_roughness_map" ), METALLIC_ROUGHNESS_SLOT );
        m_gbufferProgram.stop();
    }

    m_boxWireframeProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "debug/box_wireframe" ) ) );
    m_vctProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "fullscreen", "vct_deferred" ) ) );
    {
        m_vctProgram.use();
        m_vctProgram.setUniform( m_vctProgram.getUniformLocation( "u_gbuffer_depth" ), TEXTURE_GBUFFER_DEPTH_SLOT );
        m_vctProgram.setUniform( m_vctProgram.getUniformLocation( "u_gbuffer_albedo" ), TEXTURE_GBUFFER_ALBEDO_SLOT );
        m_vctProgram.setUniform( m_vctProgram.getUniformLocation( "u_gbuffer_position_metallic" ), TEXTURE_GBUFFER_POSITION_METALLIC_SLOT );
        m_vctProgram.setUniform( m_vctProgram.getUniformLocation( "u_gbuffer_normal_roughness" ), TEXTURE_GBUFFER_NORMAL_ROUGHNESS_SLOT );
        m_vctProgram.setUniform( m_vctProgram.getUniformLocation( "u_shadow_map" ), TEXTURE_SHADOW_MAP_SLOT );
        m_vctProgram.setUniform( m_vctProgram.getUniformLocation( "u_voxel_albedo" ), TEXTURE_VOXEL_ALBEDO_SLOT );
        m_vctProgram.setUniform( m_vctProgram.getUniformLocation( "u_voxel_normal" ), TEXTURE_VOXEL_NORMAL_SLOT );
        m_vctProgram.stop();
    }

    m_depthProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "depth" ) ) );
    m_debugTextureProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "fullscreen", "debug/texture" ) ) );

    m_voxelProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSGSPS( "voxel/voxelization" ) ) );
    {
        // set one time uniforms
        m_voxelProgram.use();
        m_voxelProgram.setUniform( m_voxelProgram.getUniformLocation( "u_albedo_map" ), ALBEDO_MAP_SLOT );
        m_voxelProgram.setUniform( m_voxelProgram.getUniformLocation( "u_shadow_map" ), TEXTURE_SHADOW_MAP_SLOT );
        m_voxelProgram.stop();
    }

    m_visualizeProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "voxel/visualization" ) ) );
    m_voxelPostProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::CS( "voxel/post" ) ) );

    // frame buffers
    createFrameBuffers();

    m_boxWireframe = CreateMeshData( geometry::MakeBoxWireFrame() );
    m_box          = CreateMeshData( geometry::MakeBox() );

    // create box quad
    {
        float points[] = {
            -1.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
        };

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
        MeshData data = CreateMeshData( *mesh.get() );
        g_meshLUT.insert( { mesh.get(), data } );
    }

    // create material
    for ( const auto& mat : scene.materials )
    {
        MaterialData matData;
        if ( !mat->albedoTexture.empty() )
        {
            matData.albedoMap.create2DImageFromFile( mat->albedoTexture.c_str() );
            matData.albedoColor = vec4( 0 );
        }
        else
        {
            matData.albedoColor = vec4( mat->albedo, 1.0f );
        }

        if ( !mat->metallicRoughnessTexture.empty() )
        {
            matData.materialMap.create2DImageFromFile( mat->metallicRoughnessTexture.c_str() );
        }
        else
        {
            matData.metallic  = mat->metallic;
            matData.roughness = mat->roughness;
        }

        if ( !mat->normalTexture.empty() )
        {
            matData.normalMap.create2DImageFromFile( mat->normalTexture.c_str() );
        }

        g_matLUT.insert( { mat.get(), matData } );
    }

    glActiveTexture( GL_TEXTURE0 + TEXTURE_SHADOW_MAP_SLOT );
    m_shadowBuffer.getDepthTexture().bind();

    glActiveTexture( GL_TEXTURE0 + TEXTURE_GBUFFER_DEPTH_SLOT );
    m_gbuffer.getDepthTexture().bind();

    glActiveTexture( GL_TEXTURE0 + TEXTURE_GBUFFER_POSITION_METALLIC_SLOT );
    m_gbuffer.getColorAttachment( 0 ).bind();

    glActiveTexture( GL_TEXTURE0 + TEXTURE_GBUFFER_NORMAL_ROUGHNESS_SLOT );
    m_gbuffer.getColorAttachment( 1 ).bind();

    glActiveTexture( GL_TEXTURE0 + TEXTURE_GBUFFER_ALBEDO_SLOT );
    m_gbuffer.getColorAttachment( 2 ).bind();

    // voxels
    glActiveTexture( GL_TEXTURE0 + TEXTURE_VOXEL_ALBEDO_SLOT );
    m_albedoVoxel.bind();

    glActiveTexture( GL_TEXTURE0 + TEXTURE_VOXEL_NORMAL_SLOT );
    m_normalVoxel.bind();
}

// void MainRenderer::visualizeVoxels()
// {
//     glEnable( GL_CULL_FACE );
//     glEnable( GL_DEPTH_TEST );

//     GlslProgram& program = m_visualizeProgram;

//     glBindVertexArray( m_box.vao );
//     program.use();
//     static const GLint location_type = program.getUniformLocation( "u_is_albedo" );
//     int isAlbedo                     = g_UIControls.drawTexture == DrawTexture::TEXTURE_VOXEL_ALBEDO;

//     program.setUniform( location_type, isAlbedo );

//     int mipLevel = g_UIControls.voxelMipLevel;

//     GpuTexture& voxelTexture = isAlbedo ? m_albedoVoxel : m_normalVoxel;

//     glBindImageTexture( 0, voxelTexture.getHandle(), mipLevel, GL_TRUE, 0,
//                         GL_READ_ONLY, voxelTexture.getFormat() );

//     int size = VOXEL_TEXTURE_SIZE >> mipLevel;
//     glDrawElementsInstanced( GL_TRIANGLES, m_box.count, GL_UNSIGNED_INT, 0, size * size * size );

//     program.stop();
// }

void MainRenderer::gbufferPass()
{
    Scene scene          = Com_GetScene();
    GlslProgram& program = m_gbufferProgram;

    m_gbuffer.bind();
    program.use();

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    static GLint location_M = program.getUniformLocation( "M" );
    // TODO: refactor draw scene
    for ( const GeometryNode& node : scene.geometryNodes )
    {
        program.setUniform( location_M, node.transform );

        for ( const Geometry& geom : node.geometries )
        {
            if ( !scene.camera.frustum.Intersect( geom.boundingBox ) )
            {
                continue;
            }

            const auto& meshPair = g_meshLUT.find( geom.pMesh );
            core_assert( meshPair != g_meshLUT.end() );
            const MeshData& drawData = meshPair->second;

            const auto& matPair = g_matLUT.find( geom.pMaterial );
            core_assert( matPair != g_matLUT.end() );
            const MaterialData& matData = matPair->second;

            m_fsMaterialBuffer.cache = matData;
            m_fsMaterialBuffer.Update();

            glBindVertexArray( drawData.vao );
            glActiveTexture( GL_TEXTURE0 + ALBEDO_MAP_SLOT );
            matData.albedoMap.bind();
            glActiveTexture( GL_TEXTURE0 + METALLIC_ROUGHNESS_SLOT );
            matData.materialMap.bind();
            glActiveTexture( GL_TEXTURE0 + NORMAL_MAP_SLOT );
            matData.normalMap.bind();

            glDrawElements( GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0 );
        }
    }

    m_gbufferProgram.stop();
    m_gbuffer.unbind();
}

void MainRenderer::shadowPass()
{
    Scene scene = Com_GetScene();
    m_shadowBuffer.bind();

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_FRONT );

    glViewport( 0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION );
    glClear( GL_DEPTH_BUFFER_BIT );
    // render scene
    m_depthProgram.use();

    static GLint PVMLocation = m_depthProgram.getUniformLocation( "u_PVM" );

    for ( const GeometryNode& node : scene.geometryNodes )
    {
        mat4 PVM = g_perframeCache.cache.LightPV * node.transform;
        m_depthProgram.setUniform( PVMLocation, PVM );
        for ( const Geometry& geom : node.geometries )
        {
            const auto& meshPair = g_meshLUT.find( geom.pMesh );
            core_assert( meshPair != g_meshLUT.end() );
            const MeshData& drawData = meshPair->second;

            glBindVertexArray( drawData.vao );
            glDrawElements( GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0 );
        }
    }

    m_shadowBuffer.unbind();

    glCullFace( GL_BACK );
}

void MainRenderer::renderToVoxelTexture()
{
    Scene scene = Com_GetScene();

    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glViewport( 0, 0, VOXEL_TEXTURE_SIZE, VOXEL_TEXTURE_SIZE );

    m_albedoVoxel.bindImageTexture( IMAGE_VOXEL_ALBEDO_SLOT );
    m_normalVoxel.bindImageTexture( IMAGE_VOXEL_NORMAL_SLOT );
    m_voxelProgram.use();
    static GLint mLocation = m_voxelProgram.getUniformLocation( "u_M" );

    for ( const GeometryNode& node : scene.geometryNodes )
    {
        m_voxelProgram.setUniform( mLocation, node.transform );
        for ( const Geometry& geom : node.geometries )
        {
            const auto& meshPair = g_meshLUT.find( geom.pMesh );
            core_assert( meshPair != g_meshLUT.end() );
            const MeshData& drawData = meshPair->second;

            const auto& matPair = g_matLUT.find( geom.pMaterial );
            core_assert( matPair != g_matLUT.end() );
            const MaterialData& matData = matPair->second;

            m_fsMaterialBuffer.cache = matData;
            m_fsMaterialBuffer.Update();

            glBindVertexArray( drawData.vao );
            glActiveTexture( GL_TEXTURE0 + ALBEDO_MAP_SLOT );
            matData.albedoMap.bind();

            glDrawElements( GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0 );
        }
    }

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    // post process
    m_voxelPostProgram.use();

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

    program.use();
    // static GLint location_texture = program.getUniformLocation("u_texture");

    static GLint location_gi_mode = program.getUniformLocation( "u_gi_mode" );
    program.setUniform( location_gi_mode, g_UIControls.voxelGiMode );

    glBindVertexArray( m_quad.vao );
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    program.stop();
}

// void MainRenderer::renderBoundingBox()
// {
//     Scene scene = Com_GetScene();

//     /// TODO: refactor
//     m_boxWireframeProgram.use();
//     static const GLint centerLocation = m_boxWireframeProgram.getUniformLocation( "u_center" );
//     static const GLint sizeLocation   = m_boxWireframeProgram.getUniformLocation( "u_size" );
//     static const GLint colorLocation  = m_boxWireframeProgram.getUniformLocation( "u_color" );

//     glBindVertexArray( m_boxWireframe.vao );

//     if ( g_UIControls.showObjectBoundingBox )
//     {
//         m_boxWireframeProgram.setUniform( colorLocation, vec3( 0, 1, 0 ) );
//         for ( const GeometryNode& node : scene.geometryNodes )
//         {
//             for ( const Geometry& geom : node.geometries )
//             {
//                 m_boxWireframeProgram.setUniform( centerLocation, geom.boundingBox.Center() );
//                 m_boxWireframeProgram.setUniform( sizeLocation, geom.boundingBox.Size() );
//                 glDrawElements( GL_LINES, m_boxWireframe.count, GL_UNSIGNED_INT, 0 );
//             }
//         }
//     }

//     if ( g_UIControls.showWorldBoundingBox )
//     {
//         m_boxWireframeProgram.setUniform( colorLocation, vec3( 0, 0, 1 ) );
//         m_boxWireframeProgram.setUniform( centerLocation, scene.boundingBox.Center() );
//         m_boxWireframeProgram.setUniform( sizeLocation, scene.boundingBox.Size() );
//         glDrawElements( GL_LINES, m_boxWireframe.count, GL_UNSIGNED_INT, 0 );
//     }
// }

void MainRenderer::renderFrameBufferTextures( const ivec2& extent )
{
    GlslProgram& program = m_debugTextureProgram;

    program.use();

    glDisable( GL_DEPTH_TEST );

    static GLint location_texture = program.getUniformLocation( "u_texture" );
    static GLint location_type    = program.getUniformLocation( "u_type" );

    constexpr float scale = 0.13f;
    int width             = static_cast<int>( scale * extent.x );
    int height            = static_cast<int>( scale * extent.y );

    glBindVertexArray( m_quad.vao );
    glViewport( 0, 0, extent.x, extent.y );

    int textureSlot = 0;
    int type        = -1;
    // normal
    switch ( g_UIControls.drawTexture )
    {
        case DrawTexture::TEXTURE_GBUFFER_SHADOW:
            textureSlot = TEXTURE_SHADOW_MAP_SLOT;
            type        = 0;
            break;
        case DrawTexture::TEXTURE_GBUFFER_DEPTH:
            textureSlot = TEXTURE_GBUFFER_DEPTH_SLOT;
            type        = 0;
            break;
        case DrawTexture::TEXTURE_GBUFFER_ALBEDO:
            textureSlot = TEXTURE_GBUFFER_ALBEDO_SLOT;
            type        = 1;
            break;
        case DrawTexture::TEXTURE_GBUFFER_NORMAL:
            textureSlot = TEXTURE_GBUFFER_NORMAL_ROUGHNESS_SLOT;
            type        = 2;
            break;
        case DrawTexture::TEXTURE_GBUFFER_METALLIC:
            textureSlot = TEXTURE_GBUFFER_POSITION_METALLIC_SLOT;
            type        = 3;
            break;
        case DrawTexture::TEXTURE_GBUFFER_ROUGHNESS:
            textureSlot = TEXTURE_GBUFFER_NORMAL_ROUGHNESS_SLOT;
            type        = 3;
            break;
        default: break;
    }

    program.setUniform( location_texture, textureSlot );
    program.setUniform( location_type, type );
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    program.stop();
}

void MainRenderer::render()
{
    Scene scene = Com_GetScene();

    // update perframe cache
    const mat4 PV                  = scene.camera.perspective() * scene.camera.view();
    g_perframeCache.cache.LightPos = scene.light.position;
    g_perframeCache.cache.CamPos   = scene.camera.position;
    g_perframeCache.cache.PV       = PV;
    g_perframeCache.cache.LightPV  = lightSpaceMatrix( scene.light.position, scene.shadowBox );
    g_perframeCache.Update();

    if ( scene.lightDirty )
    {
        shadowPass();
    }

    if ( scene.dirty || g_UIControls.forceUpdateVoxelTexture )
    {
        m_albedoVoxel.clear();
        m_normalVoxel.clear();
        renderToVoxelTexture();
    }

    ivec2 extent = MainWindow::FrameSize();
    if ( extent.x * extent.y > 0 )
    {
        // skip rendering if minimized
        scene.camera.frustum = Frustum( PV );

        glViewport( 0, 0, extent.x, extent.y );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glEnable( GL_DEPTH_TEST );
        glEnable( GL_CULL_FACE );

        gbufferPass();
        vctPass();

        // if ( g_UIControls.drawTexture == DrawTexture::TEXTURE_FINAL_IMAGE )
        // {
        // }
        // else if ( g_UIControls.drawTexture < DrawTexture::TEXTURE_GBUFFER_NONE )
        // {
        //     visualizeVoxels();
        // }
        // else
        // {
        //     renderFrameBufferTextures( extent );
        // }

        // if ( g_UIControls.showObjectBoundingBox || g_UIControls.showWorldBoundingBox )
        //     renderBoundingBox();
    }
}

void MainRenderer::createFrameBuffers()
{
    const ivec2 extent = MainWindow::FrameSize();

    m_shadowBuffer.create( SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION );

    m_gbuffer.create( extent.x, extent.y );
}

void MainRenderer::destroyGpuResources()
{
    // gpu resource
    m_voxelProgram.destroy();
    m_visualizeProgram.destroy();
    m_boxWireframeProgram.destroy();
    m_voxelPostProgram.destroy();
    m_depthProgram.destroy();
    m_debugTextureProgram.destroy();

    // render targets
    m_shadowBuffer.destroy();
    m_gbuffer.destroy();

    R_Destroy_Cbuffers();
}

}  // namespace vct
