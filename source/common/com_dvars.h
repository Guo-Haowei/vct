#include "universal/dvar_begin.h"

DVAR_STRING( scene, "Sponza/sponza.gltf" );
DVAR_FLOAT( scene_scale, 0.01f );
DVAR_VEC3( cam_pos, -8.f, 2.f, 0.f );
DVAR_VEC4( cam_cascades, 1.0f, 8.0f, 18.0f, 50.0f );
DVAR_VEC3( light_dir, 1.0f, 25.0f, 5.0f );

DVAR_INT( r_debug, 1 );
DVAR_INT( r_enableVXGI, 1 );
DVAR_INT( r_debugCSM, 0 );
DVAR_INT( r_forceVXGI, 1 );
DVAR_INT( r_shadowRes, 4096 );
DVAR_INT( r_debugTexture, 0 );
DVAR_INT( r_noTexture, 0 );

#include "universal/dvar_end.h"