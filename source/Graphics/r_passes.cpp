#include "r_passes.h"

#include <random>

#include "Base/Asserts.h"

#include "Core/com_dvars.h"
#include "Core/com_misc.h"
#include "Core/WindowManager.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "r_rendertarget.h"

#include "Graphics/PipelineStateManager.hpp"
#include "Graphics/GraphicsManager.hpp"

static GLuint g_noiseTexture;

void R_Gbuffer_Pass()
{
    auto PSO = g_pPipelineStateManager->GetPipelineState( "GBUFFER" );
    g_gfxMgr->SetPipelineState( PSO );

    Scene& scene = Com_GetScene();

    g_gbufferRT.Bind();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Frame frame;
    g_gfxMgr->DrawBatch( frame );

    g_gbufferRT.Unbind();
}

void R_SSAO_Pass()
{
    auto PSO = g_pPipelineStateManager->GetPipelineState( "SSAO" );
    g_gfxMgr->SetPipelineState( PSO );

    g_ssaoRT.Bind();

    glClear( GL_COLOR_BUFFER_BIT );

    R_DrawQuad();

    g_ssaoRT.Unbind();
}

void R_Deferred_VCT_Pass()
{
    auto PSO = g_pPipelineStateManager->GetPipelineState( "VCT" );
    g_gfxMgr->SetPipelineState( PSO );

    g_finalImageRT.Bind();

    glClear( GL_COLOR_BUFFER_BIT );

    R_DrawQuad();

    g_finalImageRT.Unbind();
}

void R_FXAA_Pass()
{
    auto PSO = g_pPipelineStateManager->GetPipelineState( "FXAA" );
    g_gfxMgr->SetPipelineState( PSO );

    g_fxaaRT.Bind();
    glClear( GL_COLOR_BUFFER_BIT );

    R_DrawQuad();

    g_fxaaRT.Unbind();
}

static float lerp( float a, float b, float f )
{
    return a + f * ( b - a );
}

static void CreateSSAOResource()
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

void R_Create_Pass_Resources()
{
    CreateSSAOResource();
}

void R_Destroy_Pass_Resources()
{
    glDeleteTextures( 1, &g_noiseTexture );
}
