#include "core/dynamic_variable/dynamic_variable_begin.h"

DVAR_INT(test_int, DVAR_FLAG_NONE, "", 100);
DVAR_FLOAT(test_float, DVAR_FLAG_NONE, "", 2.3f);
DVAR_STRING(test_string, DVAR_FLAG_NONE, "", "abc");
DVAR_VEC2(test_vec2, DVAR_FLAG_NONE, "", 1.0f, 2.0f);
DVAR_VEC3(test_vec3, DVAR_FLAG_NONE, "", 1.0f, 2.0f, 3.0f);
DVAR_VEC4(test_vec4, DVAR_FLAG_NONE, "", 1.0f, 2.0f, 3.0f, 4.0f);
DVAR_IVEC2(test_ivec2, DVAR_FLAG_NONE, "", 1, 2);
DVAR_IVEC3(test_ivec3, DVAR_FLAG_NONE, "", 1, 2, 3);
DVAR_IVEC4(test_ivec4, DVAR_FLAG_NONE, "", 1, 2, 3, 4);

#include "core/dynamic_variable/dynamic_variable_end.h"
