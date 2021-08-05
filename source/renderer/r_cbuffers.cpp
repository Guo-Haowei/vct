#include "r_cbuffers.h"

gl::ConstantBuffer<PerFrameCB> g_perframeCache;

void R_Alloc_Cbuffers()
{
    g_perframeCache.CreateAndBind( 0 );
}

void R_Destroy_Cbuffers()
{
    g_perframeCache.Destroy();
}