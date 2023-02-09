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

GpuTexture m_albedoVoxel;
GpuTexture m_normalVoxel;

void createGpuResources()
{
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

void destroyGpuResources()
{
    R_DestroyRT();
}
