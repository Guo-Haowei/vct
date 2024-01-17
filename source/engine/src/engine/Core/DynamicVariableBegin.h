#include "DynamicVariable.h"

#if defined(DEFINE_DVAR)
#define DVAR_INT(name, value)       DynamicVariable DVAR_##name
#define DVAR_FLOAT(name, value)     DynamicVariable DVAR_##name
#define DVAR_STRING(name, value)    DynamicVariable DVAR_##name
#define DVAR_VEC2(name, x, y)       DynamicVariable DVAR_##name
#define DVAR_VEC3(name, x, y, z)    DynamicVariable DVAR_##name
#define DVAR_VEC4(name, x, y, z, w) DynamicVariable DVAR_##name
#elif defined(REGISTER_DVAR)
#define DVAR_INT(name, value)       (DVAR_##name).RegisterInt(#name, value)
#define DVAR_FLOAT(name, value)     (DVAR_##name).RegisterFloat(#name, value)
#define DVAR_STRING(name, value)    (DVAR_##name).RegisterString(#name, value)
#define DVAR_VEC2(name, x, y)       (DVAR_##name).RegisterVec2(#name, x, y)
#define DVAR_VEC3(name, x, y, z)    (DVAR_##name).RegisterVec3(#name, x, y, z)
#define DVAR_VEC4(name, x, y, z, w) (DVAR_##name).RegisterVec4(#name, x, y, z, w)
#else
#define DVAR_INT(name, value)       extern DynamicVariable DVAR_##name
#define DVAR_FLOAT(name, value)     extern DynamicVariable DVAR_##name
#define DVAR_STRING(name, value)    extern DynamicVariable DVAR_##name
#define DVAR_VEC2(name, x, y)       extern DynamicVariable DVAR_##name
#define DVAR_VEC3(name, x, y, z)    extern DynamicVariable DVAR_##name
#define DVAR_VEC4(name, x, y, z, w) extern DynamicVariable DVAR_##name
#endif
