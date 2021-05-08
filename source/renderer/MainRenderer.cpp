#pragma once
#include "MainRenderer.h"

#include <iostream>
#include <unordered_map>

#include "common/Globals.h"
#include "scene/Scene.h"
#include "universal/core_assert.h"

namespace three {
namespace geometry {

// clang-format off

/**
 *        E__________________ H
 *       /|                 /|
 *      / |                / |
 *     /  |               /  |
 *   A/___|______________/D  |
 *    |   |              |   |
 *    |   |              |   |
 *    |   |              |   |
 *    |  F|______________|___|G
 *    |  /               |  /
 *    | /                | /
 *   B|/_________________|C
 *
 */
void boxWireframe(std::vector<vec3>& outPoints, std::vector<unsigned int>& outIndices, float size)
{
    enum VertexIndex  { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7 };

    outPoints.clear();
    outPoints = {
        { -size, +size, +size }, // A
        { -size, -size, +size }, // B
        { +size, -size, +size }, // C
        { +size, +size, +size }, // D
        { -size, +size, -size }, // E
        { -size, -size, -size }, // F
        { +size, -size, -size }, // G
        { +size, +size, -size }  // H
    };

    outIndices.clear();
    outIndices = {
        A, B, B, C, C, D, D, A,
        E, F, F, G, G, H, H, E,
        A, E, B, F, D, H, C, G
    };
}

void box(std::vector<vec3>& outPoints, std::vector<unsigned int>& outIndices, float size)
{
    enum VertexIndex { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7 };

    outPoints.clear();
    outPoints = {
        { -size, +size, +size }, // A
        { -size, -size, +size }, // B
        { +size, -size, +size }, // C
        { +size, +size, +size }, // D
        { -size, +size, -size }, // E
        { -size, -size, -size }, // F
        { +size, -size, -size }, // G
        { +size, +size, -size }  // H
    };

    outIndices.clear();
    outIndices = {
        A, B, D, // ABD
        D, B, C, // DBC
        E, H, F, // EHF
        H, G, F, // HGF

        D, C, G, // DCG
        D, G, H, // DGH
        A, F, B, // AFB
        A, E, F, // AEF

        A, D, H, // ADH
        A, H, E, // AHE
        B, F, G, // BFG
        B, G, C, // BGC
    };
}

void boxWithNormal(std::vector<vec3>& outPoints, std::vector<vec3>& outNomrals, std::vector<unsigned int>& outIndices, float size)
{
    outPoints.clear();
    outPoints = {
        { -size, +size, +size }, // A
        { -size, -size, +size }, // B
        { +size, -size, +size }, // C
        { +size, +size, +size }, // D

        { -size, +size, -size }, // E
        { -size, -size, -size }, // F
        { +size, -size, -size }, // G
        { +size, +size, -size }, // H

        { -size, +size, -size }, // E 8
        { -size, -size, -size }, // F 9
        { -size, -size, +size }, // B 10
        { -size, +size, +size }, // A 11

        { +size, +size, -size }, // H 12
        { +size, -size, -size }, // G 13
        { +size, -size, +size }, // C 14
        { +size, +size, +size }, // D 15

        { -size, +size, -size }, // E 16
        { -size, +size, +size }, // A 17
        { +size, +size, +size }, // D 18
        { +size, +size, -size }, // H 19

        { -size, -size, -size }, // F 20
        { -size, -size, +size }, // B 21
        { +size, -size, +size }, // C 22
        { +size, -size, -size }, // G 23
    };

    constexpr vec3 UnitX(1, 0, 0);
    constexpr vec3 UnitY(0, 1, 0);
    constexpr vec3 UnitZ(0, 0, 1);

    outNomrals.clear();
    outNomrals = {
        +UnitZ, +UnitZ, +UnitZ, +UnitZ,
        -UnitZ, -UnitZ, -UnitZ, -UnitZ,
        -UnitX, -UnitX, -UnitX, -UnitX,
        +UnitX, +UnitX, +UnitX, +UnitX,
        +UnitY, +UnitY, +UnitY, +UnitY,
        -UnitY, -UnitY, -UnitY, -UnitY,
    };

    outIndices.clear();
    outIndices = {
        0, 1, 3, 3, 1, 2,
        4, 7, 5, 7, 6, 5,
        8, 9, 11, 9, 10, 11,
        15, 14, 13, 15, 13, 12,
        16, 17, 18, 16, 18, 19,
        21, 20, 22, 20, 23, 22,
    };
}

} // namespace geometry
} // namespace three

namespace vct {

static std::unordered_map<const Mesh*, MeshData> g_meshLUT;
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

void MainRenderer::createGpuResources()
{
    vec3 center     = g_scene.boundingBox.Center();
    vec3 size       = g_scene.boundingBox.Size();
    float worldSize = std::max( size.x, std::max( size.y, size.z ) );
    float texelSize = 1.0f / static_cast<float>( VOXEL_TEXTURE_SIZE );
    float voxelSize = worldSize * texelSize;

    // create uniform buffer
    m_constantBuffer.createAndBind( UNIFORM_BUFFER_CONSTANT_SLOT );
    m_constantBuffer.cache.world_center    = center;
    m_constantBuffer.cache.world_size_half = 0.5f * worldSize;
    m_constantBuffer.cache.texel_size      = texelSize;
    m_constantBuffer.cache.voxel_size      = voxelSize;
    m_constantBuffer.update();

    m_vsPerFrameBuffer.createAndBind( UNIFORM_BUFFER_VS_PER_FRAME_SLOT );
    m_vsPerFrameBuffer.cache.lightSpace = lightSpaceMatrix( g_scene.light.position, g_scene.shadowBox );
    m_vsPerFrameBuffer.update();

    m_fsPerFrameBuffer.createAndBind( UNIFORM_BUFFER_FS_PER_FRAME_SLOT );
    m_fsPerFrameBuffer.cache.light_position  = g_scene.light.position;
    m_fsPerFrameBuffer.cache.light_color     = g_scene.light.color;
    m_fsPerFrameBuffer.cache.camera_position = g_scene.camera.position;
    m_fsPerFrameBuffer.update();

    m_fsMaterialBuffer.createAndBind( UNIFORM_BUFFER_MATERIAL_SLOT );

    // create shader
    m_gbufferProgram.createFromFiles(
        DATA_DIR "shaders/gbuffer.vert",
        DATA_DIR "shaders/gbuffer.frag" );
    {
        m_gbufferProgram.use();
        m_gbufferProgram.setUniform( m_gbufferProgram.getUniformLocation( "u_albedo_map" ), ALBEDO_MAP_SLOT );
        m_gbufferProgram.setUniform( m_gbufferProgram.getUniformLocation( "u_normal_map" ), NORMAL_MAP_SLOT );
        m_gbufferProgram.setUniform( m_gbufferProgram.getUniformLocation( "u_metallic_roughness_map" ), METALLIC_ROUGHNESS_SLOT );
        m_gbufferProgram.stop();
    }

    m_boxWireframeProgram.createFromFiles(
        DATA_DIR "shaders/debug/box_wireframe.vert",
        DATA_DIR "shaders/debug/box_wireframe.frag" );

    m_vctProgram.createFromFiles(
        DATA_DIR "shaders/fullscreen.vert",
        DATA_DIR "shaders/vct_deferred.frag" );
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

    m_depthProgram.createFromFiles(
        DATA_DIR "shaders/depth.vert",
        DATA_DIR "shaders/depth.frag" );

    m_debugTextureProgram.createFromFiles(
        DATA_DIR "shaders/fullscreen.vert",
        DATA_DIR "shaders/debug/texture.frag" );

    m_voxelProgram.createFromFiles(
        DATA_DIR "shaders/voxel/voxelization.vert",
        DATA_DIR "shaders/voxel/voxelization.frag",
        DATA_DIR "shaders/voxel/voxelization.geom" );
    {
        // set one time uniforms
        m_voxelProgram.use();
        m_voxelProgram.setUniform( m_voxelProgram.getUniformLocation( "u_albedo_map" ), ALBEDO_MAP_SLOT );
        m_voxelProgram.setUniform( m_voxelProgram.getUniformLocation( "u_shadow_map" ), TEXTURE_SHADOW_MAP_SLOT );
        m_voxelProgram.stop();
    }

    m_visualizeProgram.createFromFiles(
        DATA_DIR "shaders/voxel/visualization.vert",
        DATA_DIR "shaders/voxel/visualization.frag" );

    m_voxelPostProgram.createFromFile( DATA_DIR "shaders/voxel/post.comp" );

    // frame buffers
    createFrameBuffers();

    // create box wireframe
    {
        std::vector<vec3> points;
        std::vector<unsigned int> indices;
        three::geometry::boxWireframe( points, indices , 0.5f);

        glGenVertexArrays( 1, &m_boxWireframe.vao );
        glGenBuffers( 2, &m_boxWireframe.ebo );
        glBindVertexArray( m_boxWireframe.vao );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_boxWireframe.ebo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * indices.size(), indices.data(), GL_STATIC_DRAW );

        glBindBuffer( GL_ARRAY_BUFFER, m_boxWireframe.vbos[0] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vec3 ) * points.size(), points.data(), GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( vec3 ), 0 );
        glEnableVertexAttribArray( 0 );

        m_boxWireframe.count = static_cast<unsigned int>( indices.size() );
    }

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

    {
        std::vector<vec3> points;
        std::vector<unsigned int> indices;
        three::geometry::box( points, indices, 0.5f );

        glGenVertexArrays( 1, &m_box.vao );
        glGenBuffers( 2, &m_box.ebo );
        glBindVertexArray( m_box.vao );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_box.ebo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * indices.size(), indices.data(), GL_STATIC_DRAW );

        glBindBuffer( GL_ARRAY_BUFFER, m_box.vbos[0] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vec3 ) * points.size(), points.data(), GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( vec3 ), 0 );
        glEnableVertexAttribArray( 0 );

        m_box.count = static_cast<unsigned int>( indices.size() );
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

    // load scene
    // create mesh
    for ( const std::unique_ptr<Mesh>& mesh : g_scene.meshes )
    {
        // TODO: buffer storage
        MeshData data;
        glGenVertexArrays( 1, &data.vao );
        glGenBuffers( 4, &data.ebo );
        glBindVertexArray( data.vao );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data.ebo );
        glNamedBufferStorage( data.ebo, sizeof( uvec3 ) * mesh->faces.size(), mesh->faces.data(), 0 );

        glBindBuffer( GL_ARRAY_BUFFER, data.vbos[0] );
        glNamedBufferStorage( data.vbos[0], sizeof( vec3 ) * mesh->positions.size(), mesh->positions.data(), 0 );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( vec3 ), 0 );
        glEnableVertexAttribArray( 0 );

        glBindBuffer( GL_ARRAY_BUFFER, data.vbos[1] );
        glNamedBufferStorage( data.vbos[1], sizeof( vec3 ) * mesh->normals.size(), mesh->normals.data(), 0 );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( vec3 ), 0 );
        glEnableVertexAttribArray( 1 );

        glBindBuffer( GL_ARRAY_BUFFER, data.vbos[2] );
        glNamedBufferStorage( data.vbos[2], sizeof( vec2 ) * mesh->uvs.size(), mesh->uvs.data(), 0 );
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( vec2 ), 0 );
        glEnableVertexAttribArray( 2 );

        if ( mesh->tangents.size() != 0 )
        {
            glGenBuffers( 2, &data.vbos[3] );

            glBindBuffer( GL_ARRAY_BUFFER, data.vbos[3] );
            glNamedBufferStorage( data.vbos[3], sizeof( vec3 ) * mesh->tangents.size(), mesh->tangents.data(), 0 );
            glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof( vec3 ), 0 );
            glEnableVertexAttribArray( 3 );

            glBindBuffer( GL_ARRAY_BUFFER, data.vbos[4] );
            glNamedBufferStorage( data.vbos[4], sizeof( vec3 ) * mesh->bitangents.size(), mesh->bitangents.data(), 0 );
            glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof( vec3 ), 0 );
            glEnableVertexAttribArray( 4 );
        }

        glBindVertexArray( 0 );
        data.count = 3 * static_cast<unsigned int>( mesh->faces.size() );

        g_meshLUT.insert( { mesh.get(), data } );
    }

    // create material
    for ( const std::unique_ptr<Material>& mat : g_scene.materials )
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

void MainRenderer::visualizeVoxels()
{
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );

    GlslProgram& program = m_visualizeProgram;

    glBindVertexArray( m_box.vao );
    program.use();
    static const GLint location_type = program.getUniformLocation( "u_is_albedo" );
    int isAlbedo                     = g_UIControls.drawTexture == DrawTexture::TEXTURE_VOXEL_ALBEDO;

    program.setUniform( location_type, isAlbedo );

    int mipLevel = g_UIControls.voxelMipLevel;

    GpuTexture& voxelTexture = isAlbedo ? m_albedoVoxel : m_normalVoxel;

    glBindImageTexture( 0, voxelTexture.getHandle(), mipLevel, GL_TRUE, 0,
                        GL_READ_ONLY, voxelTexture.getFormat() );

    int size = VOXEL_TEXTURE_SIZE >> mipLevel;
    glDrawElementsInstanced( GL_TRIANGLES, m_box.count, GL_UNSIGNED_INT, 0, size * size * size );

    program.stop();
}

void MainRenderer::gbufferPass()
{
    GlslProgram& program = m_gbufferProgram;

    m_gbuffer.bind();
    program.use();

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    static GLint location_M = program.getUniformLocation( "M" );
    // TODO: refactor draw scene
    for ( const GeometryNode& node : g_scene.geometryNodes )
    {
        program.setUniform( location_M, node.transform );

        for ( const Geometry& geom : node.geometries )
        {
            if ( !g_scene.camera.frustum.Intersect( geom.boundingBox ) )
            {
                ++g_UIControls.objectOccluded;
                continue;
            }

            const auto& meshPair = g_meshLUT.find( geom.pMesh );
            core_assert( meshPair != g_meshLUT.end() );
            const MeshData& drawData = meshPair->second;

            const auto& matPair = g_matLUT.find( geom.pMaterial );
            core_assert( matPair != g_matLUT.end() );
            const MaterialData& matData = matPair->second;

            m_fsMaterialBuffer.cache = matData;
            m_fsMaterialBuffer.update();

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
    m_shadowBuffer.bind();

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_FRONT );

    glViewport( 0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION );
    glClear( GL_DEPTH_BUFFER_BIT );
    // render scene
    m_depthProgram.use();

    static GLint PVMLocation = m_depthProgram.getUniformLocation( "u_PVM" );

    for ( const GeometryNode& node : g_scene.geometryNodes )
    {
        mat4 PVM = m_vsPerFrameBuffer.cache.lightSpace * node.transform;
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
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glViewport( 0, 0, VOXEL_TEXTURE_SIZE, VOXEL_TEXTURE_SIZE );

    m_albedoVoxel.bindImageTexture( IMAGE_VOXEL_ALBEDO_SLOT );
    m_normalVoxel.bindImageTexture( IMAGE_VOXEL_NORMAL_SLOT );
    m_voxelProgram.use();
    static GLint mLocation = m_voxelProgram.getUniformLocation( "u_M" );

    for ( const GeometryNode& node : g_scene.geometryNodes )
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
            m_fsMaterialBuffer.update();

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

void MainRenderer::renderBoundingBox()
{
    /// TODO: refactor
    m_boxWireframeProgram.use();
    static const GLint centerLocation = m_boxWireframeProgram.getUniformLocation( "u_center" );
    static const GLint sizeLocation   = m_boxWireframeProgram.getUniformLocation( "u_size" );
    static const GLint colorLocation  = m_boxWireframeProgram.getUniformLocation( "u_color" );

    glBindVertexArray( m_boxWireframe.vao );

    if ( g_UIControls.showObjectBoundingBox )
    {
        m_boxWireframeProgram.setUniform( colorLocation, vec3( 0, 1, 0 ) );
        for ( const GeometryNode& node : g_scene.geometryNodes )
        {
            for ( const Geometry& geom : node.geometries )
            {
                m_boxWireframeProgram.setUniform( centerLocation, geom.boundingBox.Center() );
                m_boxWireframeProgram.setUniform( sizeLocation, geom.boundingBox.Size() );
                glDrawElements( GL_LINES, m_boxWireframe.count, GL_UNSIGNED_INT, 0 );
            }
        }
    }

    if ( g_UIControls.showWorldBoundingBox )
    {
        m_boxWireframeProgram.setUniform( colorLocation, vec3( 0, 0, 1 ) );
        m_boxWireframeProgram.setUniform( centerLocation, g_scene.boundingBox.Center() );
        m_boxWireframeProgram.setUniform( sizeLocation, g_scene.boundingBox.Size() );
        glDrawElements( GL_LINES, m_boxWireframe.count, GL_UNSIGNED_INT, 0 );
    }
}

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
    // update perframe cache
    m_fsPerFrameBuffer.cache.light_position  = g_scene.light.position;
    m_fsPerFrameBuffer.cache.camera_position = g_scene.camera.position;
    m_fsPerFrameBuffer.update();

    const mat4 PV                       = g_scene.camera.perspective() * g_scene.camera.view();
    m_vsPerFrameBuffer.cache.PV         = PV;
    m_vsPerFrameBuffer.cache.lightSpace = lightSpaceMatrix( g_scene.light.position, g_scene.shadowBox );
    m_vsPerFrameBuffer.update();

    if ( g_scene.lightDirty )
    {
        shadowPass();
    }

    if ( g_scene.dirty || g_UIControls.forceUpdateVoxelTexture )
    {
        m_albedoVoxel.clear();
        m_normalVoxel.clear();
        renderToVoxelTexture();
    }

    ivec2 extent;
    m_pWindow->getFrameExtent( &extent.x, &extent.y );

    if ( extent.x * extent.y > 0 )
    {
        // skip rendering if minimized
        g_scene.camera.frustum = Frustum( PV );

        glViewport( 0, 0, extent.x, extent.y );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glEnable( GL_DEPTH_TEST );
        glEnable( GL_CULL_FACE );

        gbufferPass();

        if ( g_UIControls.drawTexture == DrawTexture::TEXTURE_FINAL_IMAGE )
        {
            vctPass();
        }
        else if ( g_UIControls.drawTexture < DrawTexture::TEXTURE_GBUFFER_NONE )
        {
            visualizeVoxels();
        }
        else
        {
            renderFrameBufferTextures( extent );
        }

        if ( g_UIControls.showObjectBoundingBox || g_UIControls.showWorldBoundingBox )
            renderBoundingBox();
    }
}

void MainRenderer::createFrameBuffers()
{
    ivec2 extent;
    m_pWindow->getFrameExtent( &extent.x, &extent.y );

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
}

}  // namespace vct
