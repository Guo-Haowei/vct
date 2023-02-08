#pragma once
#include "gl_utils.h"

extern gl::ConstantBuffer<ConstantCB> g_constantCache;

void R_Alloc_Cbuffers();
void R_Destroy_Cbuffers();