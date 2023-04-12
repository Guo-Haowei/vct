#pragma once
#include "core_math.h"

struct dvar_t {
    enum { kMaxStringLengh = 128 };
    enum Type {
        Invalid,
        Integer,
        Float,
        Vec2,
        Vec3,
        Vec4,
        String,
        _VecBase = Vec2 - 2,
    };

    Type type_;

    union {
        char str_[kMaxStringLengh];
        int int_;
        float float_;
        struct {
            float x, y, z, w;
        } vec_;
    };

    // TODO: debug only
    char debugName_[kMaxStringLengh];
};

int Dvar_GetInt_Internal( const dvar_t& dvar );
float Dvar_GetFloat_Internal( const dvar_t& dvar );
vec4 Dvar_GetVec_Internal( const dvar_t& dvar, int n );
const char* Dvar_GetString_Internal( const dvar_t& dvar );
void* Dvar_GetPtr_Internal( dvar_t& dvar );

void Dvar_RegisterInt_Internal( dvar_t& dvar, const char* key, int value );
void Dvar_RegisterFloat_Internal( dvar_t& dvar, const char* key, float value );
void Dvar_RegisterVec_Internal( dvar_t& dvar, const char* key, float x, float y, float z, float w, int n );
void Dvar_RegisterString_Internal( dvar_t& dvar, const char* key, const char* value );

enum class DvarError {
    Ok,
    NotExisted,
    TypeMismatch,
};

dvar_t* Dvar_FindByName_Internal( const char* name );

DvarError Dvar_SetInt_Internal( dvar_t& dvar, int value );
DvarError Dvar_SetFloat_Internal( dvar_t& dvar, float value );
DvarError Dvar_SetVec_Internal( dvar_t& dvar, float x, float y, float z, float w );
DvarError Dvar_SetString_Internal( dvar_t& dvar, const char* value );
DvarError Dvar_SetFromString_Internal( dvar_t& dvar, const char* str );

DvarError Dvar_SetIntByName_Internal( const char* name, int value );
DvarError Dvar_SetFloatByName_Internal( const char* name, float value );
DvarError Dvar_SetVecByName_Internal( const char* name, float x, float y, float z, float w, int n );
DvarError Dvar_SetStringByName_Internal( const char* name, const char* value );

#define Dvar_GetInt( name )    Dvar_GetInt_Internal( DVAR_##name )
#define Dvar_GetBool( name )   ( !!Dvar_GetInt_Internal( DVAR_##name ) )
#define Dvar_GetFloat( name )  Dvar_GetFloat_Internal( DVAR_##name )
#define Dvar_GetVec2( name )   ( vec2( Dvar_GetVec_Internal( DVAR_##name, 2 ) ) )
#define Dvar_GetVec3( name )   ( vec3( Dvar_GetVec_Internal( DVAR_##name, 3 ) ) )
#define Dvar_GetVec4( name )   ( vec4( Dvar_GetVec_Internal( DVAR_##name, 4 ) ) )
#define Dvar_GetString( name ) Dvar_GetString_Internal( DVAR_##name )
#define Dvar_GetPtr( name )    Dvar_GetPtr_Internal( DVAR_##name )

#define Dvar_SetInt( name, value )       Dvar_SetInt_Internal( DVAR_##name, value )
#define Dvar_SetFloat( name, value )     Dvar_SetFloat_Internal( DVAR_##name, value )
#define Dvar_SetVec2( name, x, y )       Dvar_SetVec_Internal( DVAR_##name, x, y, 0.0f, 0.0f, 2 )
#define Dvar_SetVec3( name, x, y, z )    Dvar_SetVec_Internal( DVAR_##name, x, y, z, 0.0f, 3 )
#define Dvar_SetVec4( name, x, y, z, w ) Dvar_SetVec_Internal( DVAR_##name, x, y, z, w, 4 )
#define Dvar_SetString( name, value )    Dvar_SetString_Internal( DVAR_##name, value )
