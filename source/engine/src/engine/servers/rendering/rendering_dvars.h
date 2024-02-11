#include "core/dynamic_variable/dynamic_variable_begin.h"

// GFX
DVAR_STRING(r_backend, 0, "opengl");
DVAR_STRING(r_render_method, 0, "vxgi");
DVAR_BOOL(r_gpu_validation, 0, false);

// voxel GI
DVAR_INT(r_voxel_size, 0, 64);
DVAR_INT(r_enable_vxgi, 0, 1);
DVAR_INT(r_debugTexture, 0, 0);
DVAR_INT(r_no_texture, 0, 0);

// CSM
DVAR_INT(r_debugCSM, 0, 0);
DVAR_INT(r_shadowRes, 0, 1024 * 4);
DVAR_INT(r_enableCSM, 0, 1);

// SSAO
DVAR_INT(r_enableSsao, 0, 1);
DVAR_INT(r_ssaoKernelSize, 0, 32);
DVAR_INT(r_ssaoNoiseSize, 0, 4);
DVAR_FLOAT(r_ssaoKernelRadius, 0, 0.5f);

// c_fxaa_image
DVAR_INT(r_enableFXAA, 0, 1);

#include "core/dynamic_variable/dynamic_variable_end.h"