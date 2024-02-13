#include "dynamic_variable_begin.h"

DVAR_STRING(project, 0, "Open project at start", "");

// window
DVAR_IVEC2(window_resolution, DVAR_FLAG_SERIALIZE, "Request window resolution", 800, 600);
DVAR_IVEC2(window_position, DVAR_FLAG_SERIALIZE, "Request window position", 40, 40);

// IO
DVAR_BOOL(verbose, 0, "Print verbose log", true);

// cache
DVAR_STRING(recent_files, DVAR_FLAG_SERIALIZE, "A list of recent opened scenes", "");

// gui
DVAR_BOOL(grid_visibility, 0, "show editor grid", true);

// loader
DVAR_BOOL(force_assimp_loader, 0, "force use assimp loader", false);

#include "dynamic_variable_end.h"
