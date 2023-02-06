#pragma once
#include "gl_utils.h"

// inline include
#include "shaders/cbuffer.glsl"

extern gl::ConstantBuffer<MaterialCB> g_materialCache;
extern gl::ConstantBuffer<ConstantCB> g_constantCache;

void R_Alloc_Cbuffers();
void R_Destroy_Cbuffers();