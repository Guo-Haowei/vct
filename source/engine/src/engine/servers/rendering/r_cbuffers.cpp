#include "r_cbuffers.h"

gl::ConstantBuffer<PerFrameConstantBuffer> g_perFrameCache;
gl::ConstantBuffer<PerBatchConstantBuffer> g_perBatchCache;
gl::ConstantBuffer<PerSceneConstantBuffer> g_constantCache;

gl::ConstantBuffer<MaterialConstantBuffer> g_materialCache;
gl::ConstantBuffer<BoneConstantBuffer> g_boneCache;

void R_Alloc_Cbuffers() {
    g_perFrameCache.CreateAndBind();
    g_perBatchCache.CreateAndBind();
    g_materialCache.CreateAndBind();
    g_constantCache.CreateAndBind();
    g_boneCache.CreateAndBind();
}

void R_Destroy_Cbuffers() {
    g_perFrameCache.Destroy();
    g_materialCache.Destroy();
    g_constantCache.Destroy();
    g_perBatchCache.Destroy();
    g_boneCache.Destroy();
}