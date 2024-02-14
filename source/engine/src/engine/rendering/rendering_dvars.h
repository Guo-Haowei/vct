#include "core/dynamic_variable/dynamic_variable_begin.h"

// GFX
DVAR_STRING(r_backend, 0, "Renderer backend", "opengl");
DVAR_STRING(r_render_graph, 0, "Renderer graph", "vxgi");
DVAR_BOOL(r_gpu_validation, 0, "Enable GPU validation", true);

// voxel GI
DVAR_INT(r_voxel_size, 0, "Voxel size", 64);
DVAR_INT(r_debug_texture, 0, "", 0);
DVAR_BOOL(r_enable_vxgi, 0, "Enable VXGI", 1);
DVAR_BOOL(r_no_texture, 0, "", 0);
DVAR_FLOAT(r_world_size, 0, "", 20.0f);

// shadow
DVAR_INT(r_shadow_res, 0, "Shadow resolution", 1024 * 2);

// SSAO
DVAR_INT(r_enableSsao, 0, "", 1);
DVAR_INT(r_ssaoKernelSize, 0, "", 32);
DVAR_INT(r_ssaoNoiseSize, 0, "", 4);
DVAR_FLOAT(r_ssaoKernelRadius, 0, "", 0.5f);

// c_fxaa_image
DVAR_BOOL(r_enableFXAA, 0, "Enalbe FXAA", true);

#include "core/dynamic_variable/dynamic_variable_end.h"