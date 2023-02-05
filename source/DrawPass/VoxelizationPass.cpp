#include "VoxelizationPass.hpp"

// @TODO: remove
#include "Graphics/r_rendertarget.h"
#include "Core/com_dvars.h"
#include "glad/glad.h"
#include "Graphics/r_defines.h"

extern GpuTexture m_albedoVoxel;
extern GpuTexture m_normalVoxel;

void VoxelizationPass::Draw( Frame& frame )
{
    m_albedoVoxel.clear();
    m_normalVoxel.clear();

    const int voxelSize = Dvar_GetInt( r_voxelSize );
    glViewport( 0, 0, voxelSize, voxelSize );

    // @TODO: move to PSO
    glDisable( GL_BLEND );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

    m_albedoVoxel.bindImageTexture( IMAGE_VOXEL_ALBEDO_SLOT );
    m_normalVoxel.bindImageTexture( IMAGE_VOXEL_NORMAL_SLOT );

    auto PSO = m_pPipelineStateManager->GetPipelineState( "VOXEL" );
    m_pGraphicsManager->SetPipelineState( PSO );

    m_pGraphicsManager->DrawBatch( frame );

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    // @TODO: make another pass
    // post process
    {
        auto PSO = m_pPipelineStateManager->GetPipelineState( "VOXEL_POST" );
        m_pGraphicsManager->SetPipelineState( PSO );

        constexpr GLuint workGroupX = 512;
        constexpr GLuint workGroupY = 512;
        const GLuint workGroupZ = ( voxelSize * voxelSize * voxelSize ) / ( workGroupX * workGroupY );

        glDispatchCompute( workGroupX, workGroupY, workGroupZ );
        glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

        m_albedoVoxel.bind();
        m_albedoVoxel.genMipMap();
        m_normalVoxel.bind();
        m_normalVoxel.genMipMap();
    }
}