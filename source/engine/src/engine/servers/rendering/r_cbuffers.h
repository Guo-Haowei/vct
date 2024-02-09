#pragma once
#include "gl_utils.h"
#include "r_defines.h"

// inline include
#include "cbuffer.glsl.h"

extern gl::ConstantBuffer<PerFrameConstantBuffer> g_perFrameCache;
extern gl::ConstantBuffer<PerBatchConstantBuffer> g_perBatchCache;
extern gl::ConstantBuffer<MaterialConstantBuffer> g_materialCache;
extern gl::ConstantBuffer<PerSceneConstantBuffer> g_constantCache;

void R_Alloc_Cbuffers();
void R_Destroy_Cbuffers();