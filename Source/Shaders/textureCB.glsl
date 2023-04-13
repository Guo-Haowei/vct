#ifdef __cplusplus
#include "universal/universal.h"
using sampler2D = unsigned long long;
static_assert(sizeof(sampler2D) == 8);
#else
#extension GL_ABR_bindless_texture : require
#endif

#ifndef MAX_SAMPLERS
#define MAX_SAMPLERS 128
#endif

#ifdef __cplusplus
struct ConstantCB
#else
layout(std140, binding = 4) uniform ConstantCB
#endif
{
    sampler2D tex[MAX_SAMPLERS];
};