#pragma once
#include "MainRenderer.h"
#include <random>

#include "Base/Asserts.h"

#include "Core/com_dvars.h"
#include "Core/geometry.h"
#include "r_rendertarget.h"

#include "Manager/SceneManager.hpp"

#include "DrawPass/BaseDrawPass.hpp"

static std::vector<std::shared_ptr<MeshData>> g_meshdata;
static std::vector<std::shared_ptr<MaterialData>> g_materialdata;

static GLuint g_noiseTexture;
MainRenderer renderer;
GpuTexture m_albedoVoxel;
GpuTexture m_normalVoxel;

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

static float lerp( float a, float b, float f )
{
    return a + f * ( b - a );
}

static void R_Create_Pass_Resources()
{
    // generate sample kernel
    std::uniform_real_distribution<float> randomFloats( 0.0f, 1.0f );  // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec4> ssaoKernel;
    const int kernelSize = Dvar_GetInt( r_ssaoKernelSize );
    for ( int i = 0; i < kernelSize; ++i ) {
        // [-1, 1], [-1, 1], [0, 1]
        glm::vec3 sample( randomFloats( generator ) * 2.0 - 1.0, randomFloats( generator ) * 2.0 - 1.0, randomFloats( generator ) );
        sample = glm::normalize( sample );
        sample *= randomFloats( generator );
        float scale = float( i ) / kernelSize;

        scale = lerp( 0.1f, 1.0f, scale * scale );
        sample *= scale;
        ssaoKernel.emplace_back( vec4( sample, 0.0f ) );
    }

    memset( &g_constantCache.cache.SSAOKernels, 0, sizeof( g_constantCache.cache.SSAOKernels ) );
    memcpy( &g_constantCache.cache.SSAOKernels, ssaoKernel.data(), sizeof( ssaoKernel.front() ) * ssaoKernel.size() );

    // generate noise texture
    const int noiseSize = Dvar_GetInt( r_ssaoNoiseSize );

    std::vector<glm::vec3> ssaoNoise;
    for ( int i = 0; i < noiseSize * noiseSize; ++i ) {
        glm::vec3 noise( randomFloats( generator ) * 2.0 - 1.0, randomFloats( generator ) * 2.0 - 1.0, 0.0f );
        noise = glm::normalize( noise );
        ssaoNoise.emplace_back( noise );
    }
    unsigned int noiseTexture;
    glGenTextures( 1, &noiseTexture );
    glBindTexture( GL_TEXTURE_2D, noiseTexture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, ssaoNoise.data() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    g_constantCache.cache.NoiseMap = gl::MakeTextureResident( noiseTexture );
    g_noiseTexture = noiseTexture;
}

static void R_Destroy_Pass_Resources()
{
    glDeleteTextures( 1, &g_noiseTexture );
}

void MainRenderer::createGpuResources()
{
    R_Create_Pass_Resources();

    R_Alloc_Cbuffers();
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
    for ( const auto& mesh : scene.m_meshes ) {
        g_meshdata.emplace_back( CreateMeshData( *mesh.get() ) );
        mesh->gpuResource = g_meshdata.back().get();
    }

    // create material
    ASSERT( scene.m_materials.size() < array_length( g_constantCache.cache.AlbedoMaps ) );

    for ( int idx = 0; idx < scene.m_materials.size(); ++idx ) {
        const auto& mat = scene.m_materials.at( idx );

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

    {
        int i = 0;
        g_constantCache.cache.OverlayPositions[i++] = vec4( vec2( 0 ), vec2( 1 ) );
        constexpr float s = 0.14f;
        for ( float h = 1.0f - s; i < NUM_OVERLAYS; ) {
            g_constantCache.cache.OverlayPositions[i++] = vec4( vec2( 1.0f - s, h ), vec2( s - 0.01f ) );
            h -= 2 * s;
        }
    }

    g_constantCache.Update();
}

// @TODO: make another pass
#if 0
void MainRenderer::visualizeVoxels()
{
    auto PSO = g_pPipelineStateManager->GetPipelineState( "VOXEL_VIS" );
    g_gfxMgr->SetPipelineState( PSO );
    glBindVertexArray( m_box->vao );
    const int size = Dvar_GetInt( r_voxelSize );
    glDrawElementsInstanced( GL_TRIANGLES, m_box->count, GL_UNSIGNED_INT, 0, size * size * size );
}
#endif

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

void MainRenderer::destroyGpuResources()
{
    R_DestroyRT();

    R_Destroy_Cbuffers();

    R_Destroy_Pass_Resources();
}
