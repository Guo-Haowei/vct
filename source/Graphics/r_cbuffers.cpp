#include "r_cbuffers.h"

gl::ConstantBuffer<PerFrameCB> g_perFrameCache;
gl::ConstantBuffer<MaterialCB> g_materialCache;
gl::ConstantBuffer<ConstantCB> g_constantCache;

void R_Alloc_Cbuffers()
{
    g_perFrameCache.CreateAndBind( 0 );
    g_materialCache.CreateAndBind( 2 );
    g_constantCache.CreateAndBind( 3 );
}

void R_Destroy_Cbuffers()
{
    g_perFrameCache.Destroy();
    g_materialCache.Destroy();
    g_constantCache.Destroy();
}