#include "dvar_api.h"
#if defined( DEFINE_DVAR )
#define DVAR_INT( name, value )       dvar_t DVAR_##name
#define DVAR_FLOAT( name, value )     dvar_t DVAR_##name
#define DVAR_VEC2( name, x, y )       dvar_t DVAR_##name
#define DVAR_VEC3( name, x, y, z )    dvar_t DVAR_##name
#define DVAR_VEC4( name, x, y, z, w ) dvar_t DVAR_##name
#define DVAR_STRING( name, value )    dvar_t DVAR_##name
#elif defined( REGISTER_DVAR )
#define DVAR_INT( name, value )       Dvar_RegisterInt_Internal( DVAR_##name, #name, value )
#define DVAR_FLOAT( name, value )     Dvar_RegisterFloat_Internal( DVAR_##name, #name, value )
#define DVAR_VEC2( name, x, y )       Dvar_RegisterVec_Internal( DVAR_##name, #name, x, y, 0.0f, 0.0f, 2 )
#define DVAR_VEC3( name, x, y, z )    Dvar_RegisterVec_Internal( DVAR_##name, #name, x, y, z, 0.0f, 3 )
#define DVAR_VEC4( name, x, y, z, w ) Dvar_RegisterVec_Internal( DVAR_##name, #name, x, y, z, w, 4 )
#define DVAR_STRING( name, value )    Dvar_RegisterString_Internal( DVAR_##name, #name, value )
#else
#define DVAR_INT( name, value )       extern dvar_t DVAR_##name
#define DVAR_FLOAT( name, value )     extern dvar_t DVAR_##name
#define DVAR_VEC2( name, x, y )       extern dvar_t DVAR_##name
#define DVAR_VEC3( name, x, y, z )    extern dvar_t DVAR_##name
#define DVAR_VEC4( name, x, y, z, w ) extern dvar_t DVAR_##name
#define DVAR_STRING( name, value )    extern dvar_t DVAR_##name
#endif
