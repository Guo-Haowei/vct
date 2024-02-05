#include "dynamic_variable/dynamic_variable_begin.h"

DVAR_STRING(scene, 0, "");

// @TODO: refactor
DVAR_STRING(r_backend, 0, "opengl");
DVAR_VEC2(window_resolution, 0, 800, 600);

// camera
DVAR_VEC3(cam_pos, 0, -8.f, 2.f, 0.f);
DVAR_VEC4(cam_cascades, 0, 0.1f, 8.0f, 18.0f, 50.0f);
DVAR_FLOAT(cam_fov, 0, 60.0f);

// light
DVAR_VEC3(light_dir, 0, -8.0f, 25.0f, 5.0f);
DVAR_FLOAT(light_power, 0, 20.0f);

DVAR_BOOL(r_gpu_validation, 0, false);
DVAR_INT(r_enableVXGI, 0, 1);
DVAR_INT(r_forceVXGI, 0, 1);
DVAR_INT(r_debugTexture, 0, 0);
DVAR_INT(r_noTexture, 0, 0);

// voxel GI
DVAR_INT(r_voxelSize, 0, 64);

// CSM
DVAR_INT(r_debugCSM, 0, 0);
DVAR_INT(r_shadowRes, 0, 1024);
DVAR_INT(r_enableCSM, 0, 1);

// SSAO
DVAR_INT(r_enableSsao, 0, 1);
DVAR_INT(r_ssaoKernelSize, 0, 32);
DVAR_INT(r_ssaoNoiseSize, 0, 4);
DVAR_FLOAT(r_ssaoKernelRadius, 0, 0.5f);

// FXAA
DVAR_INT(r_enableFXAA, 0, 1);

#include "dynamic_variable/dynamic_variable_end.h"
