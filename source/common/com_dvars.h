#include "universal/dvar_begin.h"

DVAR_STRING( scene, "Sponza/sponza.gltf" );
DVAR_FLOAT( scene_scale, 0.01f );

// window
DVAR_INT( wnd_width, 0 );
DVAR_INT( wnd_height, 0 );
DVAR_INT( wnd_frameless, 0 );

// camera
DVAR_VEC3( cam_pos, -8.f, 2.f, 0.f );
DVAR_VEC4( cam_cascades, 0.1f, 8.0f, 18.0f, 50.0f );
DVAR_FLOAT( cam_fov, 60.0f );

// light
DVAR_VEC3( light_dir, -8.0f, 25.0f, 5.0f );
DVAR_FLOAT( light_power, 20.0f );

DVAR_INT( r_debug, 1 );
DVAR_INT( r_enableVXGI, 1 );
DVAR_INT( r_forceVXGI, 1 );
DVAR_INT( r_debugTexture, 0 );
DVAR_INT( r_noTexture, 0 );

// voxel GI
DVAR_INT( r_voxelSize, 64 );

// CSM
DVAR_INT( r_debugCSM, 0 );
DVAR_INT( r_shadowRes, 1024 * 4 );
DVAR_INT( r_enableCSM, 1 );

// SSAO
DVAR_INT( r_enableSsao, 1 );
DVAR_INT( r_ssaoKernelSize, 32 );
DVAR_INT( r_ssaoNoiseSize, 4 );
DVAR_FLOAT( r_ssaoKernelRadius, 0.5f );

// FXAA
DVAR_INT( r_enableFXAA, 1 );

// SSR
DVAR_INT( r_mirrorFloor, 0 );  // for testing reflection

#include "universal/dvar_end.h"