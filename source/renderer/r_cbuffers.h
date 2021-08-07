#pragma once
#include "gl_utils.h"
#include "universal/core_math.h"

struct PerFrameCB {
    mat4 PV;
    mat4 PVM;
    mat4 LightPV;
    vec3 CamPos;
    uint32_t seed;
    vec3 SunDir;
    float perframe_pad1;
    vec3 LightColor;
    float perframe_pad2;
};

static_assert( sizeof( PerFrameCB ) % 16 == 0 );

extern gl::ConstantBuffer<PerFrameCB> g_perframeCache;

void R_Alloc_Cbuffers();
void R_Destroy_Cbuffers();
