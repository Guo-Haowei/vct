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
DVAR_BOOL(grid_visibility, 0, true);

// loader
DVAR_BOOL(force_assimp_loader, 0, false);

#include "dynamic_variable_end.h"
