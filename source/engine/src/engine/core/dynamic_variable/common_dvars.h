#include "dynamic_variable_begin.h"

// @TODO: move to somewhere else
DVAR_STRING(scene, DVAR_FLAG_NONE, "Request a scene to load when the app starts", "");

// window
DVAR_IVEC2(window_resolution, DVAR_FLAG_SERIALIZE | DVAR_FLAG_DESERIALIZE, "Request window resolution", 800, 600);
DVAR_IVEC2(window_position, DVAR_FLAG_SERIALIZE | DVAR_FLAG_DESERIALIZE, "Request window position", 40, 40);

// IO
DVAR_BOOL(verbose, 0, "Print verbose log", true);

// cache
DVAR_STRING(recent_files, DVAR_FLAG_SERIALIZE | DVAR_FLAG_DESERIALIZE, "A list of recent opened scenes", "");
DVAR_BOOL(delete_dvar_cache, 0, "Delete serialized dvar file", false);

// gui
DVAR_BOOL(grid_visibility, 0, "show editor grid", true);

// loader
DVAR_BOOL(force_assimp_loader, 0, "force use assimp loader", false);

#include "dynamic_variable_end.h"
