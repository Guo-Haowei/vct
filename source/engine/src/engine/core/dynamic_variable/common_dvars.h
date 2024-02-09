#include "dynamic_variable_begin.h"

DVAR_STRING(scene, DVAR_FLAG_NONE, "");

// window
DVAR_IVEC2(window_resolution, DVAR_FLAG_SERIALIZE | DVAR_FLAG_DESERIALIZE, 800, 600);
DVAR_IVEC2(window_position, DVAR_FLAG_SERIALIZE | DVAR_FLAG_DESERIALIZE, 40, 40);

// IO
DVAR_BOOL(verbose, DVAR_FLAG_NONE, false);

// cache
DVAR_STRING(recent_files, DVAR_FLAG_SERIALIZE | DVAR_FLAG_DESERIALIZE, "");
DVAR_BOOL(delete_dvar_cache, DVAR_FLAG_NONE, false);

// gui
DVAR_BOOL(grid_visibility, 0, false);

// loader
DVAR_BOOL(force_assimp_loader, 0, false);

// GFX
DVAR_STRING(r_backend, 0, "opengl");
DVAR_BOOL(r_gpu_validation, 0, false);

DVAR_INT(r_enableVXGI, 0, 1);
DVAR_INT(r_forceVXGI, 0, 1);
DVAR_INT(r_debugTexture, 0, 0);
DVAR_INT(r_noTexture, 0, 0);

// voxel GI
DVAR_INT(r_voxelSize, 0, 64);

// CSM
DVAR_INT(r_debugCSM, 0, 0);
DVAR_INT(r_shadowRes, 0, 1024 * 4);
DVAR_INT(r_enableCSM, 0, 1);

// SSAO
DVAR_INT(r_enableSsao, 0, 1);
DVAR_INT(r_ssaoKernelSize, 0, 32);
DVAR_INT(r_ssaoNoiseSize, 0, 4);
DVAR_FLOAT(r_ssaoKernelRadius, 0, 0.5f);

// FXAA
DVAR_INT(r_enableFXAA, 0, 1);

#include "dynamic_variable_end.h"
