#include "universal/dvar_begin.h"

DVAR_STRING( scene, "Sponza/sponza.gltf" );
DVAR_FLOAT( scene_scale, 0.01f );
DVAR_VEC3( cam_pos, 0.0f, 5.0f, 0.0f );
DVAR_VEC4( cam_cascades, 1.0f, 5.0f, 20.0f, 100.0f );
DVAR_INT( r_debug, 1 );

#include "universal/dvar_end.h"