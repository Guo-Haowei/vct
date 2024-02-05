#include "dynamic_variable.h"

// clang-format off
#if defined(DEFINE_DVAR)
#define DVAR_BOOL(NAME, FLAGS, VALUE)		vct::DynamicVariable DVAR_##NAME { VARIANT_TYPE_INT,	FLAGS }
#define DVAR_INT(NAME, FLAGS, VALUE)		vct::DynamicVariable DVAR_##NAME { VARIANT_TYPE_INT,	FLAGS }
#define DVAR_FLOAT(NAME, FLAGS, VALUE)		vct::DynamicVariable DVAR_##NAME { VARIANT_TYPE_FLOAT,	FLAGS }
#define DVAR_STRING(NAME, FLAGS, VALUE)		vct::DynamicVariable DVAR_##NAME { VARIANT_TYPE_STRING,	FLAGS }
#define DVAR_VEC2(NAME, FLAGS, X, Y)		vct::DynamicVariable DVAR_##NAME { VARIANT_TYPE_VEC2,	FLAGS }
#define DVAR_VEC3(NAME, FLAGS, X, Y, Z)		vct::DynamicVariable DVAR_##NAME { VARIANT_TYPE_VEC3,	FLAGS }
#define DVAR_VEC4(NAME, FLAGS, X, Y, Z, W)	vct::DynamicVariable DVAR_##NAME { VARIANT_TYPE_VEC4,	FLAGS }
#elif defined(REGISTER_DVAR)
#define DVAR_BOOL(NAME, FLAGS, VALUE)		(DVAR_##NAME).register_int		(#NAME, !!(VALUE))
#define DVAR_INT(NAME, FLAGS, VALUE)		(DVAR_##NAME).register_int		(#NAME, VALUE)
#define DVAR_FLOAT(NAME, FLAGS, VALUE)		(DVAR_##NAME).register_float	(#NAME, VALUE)
#define DVAR_STRING(NAME, FLAGS, VALUE)		(DVAR_##NAME).register_string	(#NAME, VALUE)
#define DVAR_VEC2(NAME, FLAGS, X, Y)		(DVAR_##NAME).register_vec2		(#NAME, X, Y)
#define DVAR_VEC3(NAME, FLAGS, X, Y, Z)		(DVAR_##NAME).register_vec3		(#NAME, X, Y, Z)
#define DVAR_VEC4(NAME, FLAGS, X, Y, Z, W)	(DVAR_##NAME).register_vec4		(#NAME, X, Y, Z, W)
#else
#define DVAR_BOOL(NAME, FLAGS, VALUE)		extern vct::DynamicVariable DVAR_##NAME
#define DVAR_INT(NAME, FLAGS, VALUE)		extern vct::DynamicVariable DVAR_##NAME
#define DVAR_FLOAT(NAME, FLAGS, VALUE)		extern vct::DynamicVariable DVAR_##NAME
#define DVAR_STRING(NAME, FLAGS, VALUE)		extern vct::DynamicVariable DVAR_##NAME
#define DVAR_VEC2(NAME, FLAGS, X, Y)		extern vct::DynamicVariable DVAR_##NAME
#define DVAR_VEC3(NAME, FLAGS, X, Y, Z)		extern vct::DynamicVariable DVAR_##NAME
#define DVAR_VEC4(NAME, FLAGS, X, Y, Z, W)	extern vct::DynamicVariable DVAR_##NAME
#endif
// clang-format on
