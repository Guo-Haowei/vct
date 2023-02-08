#include "r_cbuffers.h"

gl::ConstantBuffer<ConstantCB> g_constantCache;

void R_Alloc_Cbuffers()
{
    g_constantCache.CreateAndBind( 3 );
}

void R_Destroy_Cbuffers()
{
    g_constantCache.Destroy();
}