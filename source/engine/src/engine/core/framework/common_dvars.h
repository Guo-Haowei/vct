#include "core/dynamic_variable/dynamic_variable_begin.h"

DVAR_STRING(project, 0, "Open project at start", "");
DVAR_BOOL(test_physics, 0, "Open physics test scene", false);

// window
DVAR_IVEC2(window_resolution, DVAR_FLAG_SERIALIZE, "Request window resolution", 800, 600);
DVAR_IVEC2(window_position, DVAR_FLAG_SERIALIZE, "Request window position", 40, 40);

// IO
DVAR_BOOL(verbose, 0, "Print verbose log", true);

// cache
DVAR_STRING(recent_files, DVAR_FLAG_SERIALIZE, "A list of recent opened scenes", "");

// gui
DVAR_BOOL(grid_visibility, 0, "show editor grid", true);

#include "core/dynamic_variable/dynamic_variable_end.h"
