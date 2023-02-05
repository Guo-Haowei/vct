#include "r_cbuffers.h"

gl::ConstantBuffer<MaterialCB> g_materialCache;
gl::ConstantBuffer<ConstantCB> g_constantCache;

void R_Alloc_Cbuffers()
{
    g_materialCache.CreateAndBind( 2 );
    g_constantCache.CreateAndBind( 3 );
}

void R_Destroy_Cbuffers()
{
    g_materialCache.Destroy();
    g_constantCache.Destroy();
}