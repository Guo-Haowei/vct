#pragma once
#include "gl_utils.h"
#include "r_defines.h"
#include "universal/core_math.h"

struct PerFrameCB {
    mat4 View;
    mat4 PV;
    mat4 PVM;
    vec3 CamPos;
    float perframe_pad0;
    vec3 SunDir;
    float perframe_pad1;
    vec3 LightColor;
    float perframe_pad2;
    vec4 CascadedClipZ;
    mat4 LightPVs[NUM_CASCADES];
};

static_assert( sizeof( PerFrameCB ) % 16 == 0 );

extern gl::ConstantBuffer<PerFrameCB> g_perframeCache;

void R_Alloc_Cbuffers();
void R_Destroy_Cbuffers();
