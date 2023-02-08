#pragma once
#include "MainRenderer.h"
#include <random>

#include "Base/Asserts.h"

#include "Core/com_dvars.h"
#include "Core/geometry.h"
#include "r_rendertarget.h"

#include "Manager/SceneManager.hpp"

#include "DrawPass/BaseDrawPass.hpp"

using std::string;

static std::vector<std::shared_ptr<MaterialData>> g_materialdata;

GpuTexture m_albedoVoxel;
GpuTexture m_normalVoxel;

void createGpuResources()
{
    R_Alloc_Cbuffers();

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

    g_constantCache.cache.ShadowMap = gl::MakeTextureResident( g_shadowRT.GetDepthTexture().GetHandle() );
    g_constantCache.cache.VoxelAlbedoMap = gl::MakeTextureResident( m_albedoVoxel.GetHandle() );
    g_constantCache.cache.VoxelNormalMap = gl::MakeTextureResident( m_normalVoxel.GetHandle() );

    {
        constexpr float s = 0.14f;
        float h = 1.0f - s;
        int i = 0;
        g_constantCache.cache.OverlayPositions[i++] = vec4( vec2( 1.0f - s, h ), vec2( s - 0.01f ) );
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

void destroyGpuResources()
{
    R_DestroyRT();

    R_Destroy_Cbuffers();
}
