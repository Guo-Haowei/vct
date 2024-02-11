#include "dynamic_variable.h"

// clang-format off
#if defined(DEFINE_DVAR)
#define DVAR_BOOL(name, flags, desc, value)			vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_INT,	flags, desc }
#define DVAR_INT(name, flags, desc, value)			vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_INT,	flags, desc }
#define DVAR_FLOAT(name, flags, desc, value)		vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_FLOAT,	flags, desc }
#define DVAR_STRING(name, flags, desc, value)		vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_STRING,flags, desc }
#define DVAR_VEC2(name, flags, desc, x, y)			vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_VEC2,	flags, desc }
#define DVAR_VEC3(name, flags, desc, x, y, z)		vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_VEC3,	flags, desc }
#define DVAR_VEC4(name, flags, desc, x, y, z, w)	vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_VEC4,	flags, desc }
#define DVAR_IVEC2(name, flags, desc, x, y)			vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_IVEC2,	flags, desc }
#define DVAR_IVEC3(name, flags, desc, x, y, z)		vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_IVEC3,	flags, desc }
#define DVAR_IVEC4(name, flags, desc, x, y, z, w)	vct::DynamicVariable DVAR_##name { vct::VARIANT_TYPE_IVEC4,	flags, desc }
#elif defined(REGISTER_DVAR)
#define DVAR_BOOL(name, flags, desc, value)			(DVAR_##name).register_int		(#name, !!(value))
#define DVAR_INT(name, flags, desc, value)			(DVAR_##name).register_int		(#name, value)
#define DVAR_FLOAT(name, flags, desc, value)		(DVAR_##name).register_float	(#name, value)
#define DVAR_STRING(name, flags, desc, value)		(DVAR_##name).register_string	(#name, value)
#define DVAR_VEC2(name, flags, desc, x, y)			(DVAR_##name).register_vec2		(#name, x, y)
#define DVAR_VEC3(name, flags, desc, x, y, z)		(DVAR_##name).register_vec3		(#name, x, y, z)
#define DVAR_VEC4(name, flags, desc, x, y, z, w)	(DVAR_##name).register_vec4		(#name, x, y, z, w)
#define DVAR_IVEC2(name, flags, desc, x, y)			(DVAR_##name).register_ivec2	(#name, x, y)
#define DVAR_IVEC3(name, flags, desc, x, y, z)		(DVAR_##name).register_ivec3	(#name, x, y, z)
#define DVAR_IVEC4(name, flags, desc, x, y, z, w)	(DVAR_##name).register_ivec4	(#name, x, y, z, w)
#else
#define DVAR_BOOL(name, ...)	extern vct::DynamicVariable DVAR_##name
#define DVAR_INT(name, ...)		extern vct::DynamicVariable DVAR_##name
#define DVAR_FLOAT(name, ...)	extern vct::DynamicVariable DVAR_##name
#define DVAR_STRING(name, ...)	extern vct::DynamicVariable DVAR_##name
#define DVAR_VEC2(name, ...)	extern vct::DynamicVariable DVAR_##name
#define DVAR_VEC3(name, ...)	extern vct::DynamicVariable DVAR_##name
#define DVAR_VEC4(name, ...)	extern vct::DynamicVariable DVAR_##name
#define DVAR_IVEC2(name, ...)	extern vct::DynamicVariable DVAR_##name
#define DVAR_IVEC3(name, ...)	extern vct::DynamicVariable DVAR_##name
#define DVAR_IVEC4(name, ...)	extern vct::DynamicVariable DVAR_##name
#endif
// clang-format on
