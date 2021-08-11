#include "dvar_api.h"

#include <string>
#include <unordered_map>

#include "core_assert.h"
#include "print.h"

static std::unordered_map<std::string, dvar_t*> s_dvarLookupTable;

#define DVAR_VERBOSE IN_USE
#if USING( DVAR_VERBOSE )
#define DVAR_PRINTF( fmt, ... ) Com_Printf( "[dvar] " fmt, ##__VA_ARGS__ )
#else
#define DVAR_PRINTF( ... ) ( (void)0 )
#endif

static void RegisterDvar_Internal( const char* key, dvar_t* dvar )
{
    const std::string keyStr( key );
    auto it = s_dvarLookupTable.find( keyStr );
    if ( it != s_dvarLookupTable.end() )
    {
        Com_PrintError( "[dvar] duplicated dvar %s detected", key );
    }

    strncpy( dvar->debugName_, key, dvar_t::kMaxStringLengh );
    dvar->debugName_[dvar_t::kMaxStringLengh - 1] = 0;

    s_dvarLookupTable.insert( std::make_pair( keyStr, dvar ) );
    const auto& v = dvar->vec_;
    switch ( dvar->type_ )
    {
        case dvar_t::Integer:
            DVAR_PRINTF( "register dvar '%s'(int) %d", key, dvar->int_ );
            break;
        case dvar_t::Float:
            DVAR_PRINTF( "register dvar '%s'(float) %f", key, dvar->float_ );
            break;
        case dvar_t::String:
            DVAR_PRINTF( "register dvar '%s'(string) \"%s\"", key, dvar->str_ );
            break;
        case dvar_t::Vec2:
            DVAR_PRINTF( "register dvar '%s'(vec2) { %f, %f }", key, v.x, v.y );
            break;
        case dvar_t::Vec3:
            DVAR_PRINTF( "register dvar '%s'(vec3) { %f, %f, %f }", key, v.x, v.y, v.z );
            break;
        case dvar_t::Vec4:
            DVAR_PRINTF( "register dvar '%s'(vec4) { %f, %f, %f, %f }", key, v.x, v.y, v.z, v.w );
            break;
        default:
            panic( "Unknown dvar type %d", static_cast<int>( dvar->type_ ) );
            break;
    }
}

static void SafeCopyDvarString( dvar_t& dvar, const char* value )
{
    const size_t len = strlen( value );
    if ( len > dvar_t::kMaxStringLengh - 1 )
    {
        panic( "string [%s] too long", value );
    }

    strcpy( dvar.str_, value );
}

void Dvar_RegisterInt_Internal( dvar_t& dvar, const char* key, int value )
{
    dvar.type_ = dvar_t::Integer;
    dvar.int_  = value;
    RegisterDvar_Internal( key, &dvar );
}

void Dvar_RegisterFloat_Internal( dvar_t& dvar, const char* key, float value )
{
    dvar.type_  = dvar_t::Float;
    dvar.float_ = value;
    RegisterDvar_Internal( key, &dvar );
}

void Dvar_RegisterVec_Internal( dvar_t& dvar, const char* key, float x, float y, float z, float w, int n )
{
    core_assertrange( n, 2, 4 );
    dvar.type_  = static_cast<dvar_t::Type>( dvar_t::_VecBase + n );
    dvar.vec_.x = x;
    dvar.vec_.y = y;
    dvar.vec_.z = z;
    dvar.vec_.w = w;
    RegisterDvar_Internal( key, &dvar );
}

void Dvar_RegisterString_Internal( dvar_t& dvar, const char* key, const char* value )
{
    dvar.type_ = dvar_t::String;
    SafeCopyDvarString( dvar, value );
    RegisterDvar_Internal( key, &dvar );
}

int Dvar_GetInt_Internal( const dvar_t& dvar )
{
    core_assert( dvar.type_ == dvar_t::Integer );
    return dvar.int_;
}

float Dvar_GetFloat_Internal( const dvar_t& dvar )
{
    core_assert( dvar.type_ == dvar_t::Float );
    return dvar.float_;
}

void* Dvar_GetPtr_Internal( dvar_t& dvar )
{
    return &dvar.int_;
}

vec4 Dvar_GetVec_Internal( const dvar_t& dvar, int n )
{
    core_assert( dvar.type_ == static_cast<dvar_t::Type>( dvar_t::_VecBase + n ) );
    return vec4( dvar.vec_.x, dvar.vec_.y, dvar.vec_.z, dvar.vec_.w );
}

const char* Dvar_GetString_Internal( const dvar_t& dvar )
{
    core_assert( dvar.type_ == dvar_t::String );
    return dvar.str_;
}

dvar_t* Dvar_FindByName_Internal( const char* name )
{
    auto it = s_dvarLookupTable.find( name );
    if ( it == s_dvarLookupTable.end() )
    {
        return nullptr;
    }
    return it->second;
}

DvarError Dvar_SetInt_Internal( dvar_t& dvar, int value )
{
    if ( dvar.type_ != dvar_t::Integer )
    {
        return DvarError::TypeMismatch;
    }

    dvar.int_ = value;
    return DvarError::Ok;
}

DvarError Dvar_SetFloat_Internal( dvar_t& dvar, float value )
{
    if ( dvar.type_ != dvar_t::Float )
    {
        return DvarError::TypeMismatch;
    }

    dvar.float_ = value;
    return DvarError::Ok;
}

DvarError Dvar_SetVec_Internal( dvar_t& dvar, float x, float y, float z, float w )
{
    switch ( dvar.type_ )
    {
        case dvar_t::Vec2:
            dvar.vec_.x = x;
            dvar.vec_.y = y;
        case dvar_t::Vec3:
            dvar.vec_.z = z;
        case dvar_t::Vec4:
            dvar.vec_.w = w;
            return DvarError::Ok;
        default:
            return DvarError::TypeMismatch;
    }
}

DvarError Dvar_SetString_Internal( dvar_t& dvar, const char* value )
{
    if ( dvar.type_ != dvar_t::String )
    {
        return DvarError::TypeMismatch;
    }

    SafeCopyDvarString( dvar, value );
    return DvarError::Ok;
}

DvarError Dvar_SetFromString_Internal( dvar_t& dvar, const char* str )
{
    auto& v = dvar.vec_;
    int n   = 0;
    switch ( dvar.type_ )
    {
        case dvar_t::Integer:
            dvar.int_ = atoi( str );
            DVAR_PRINTF( "change dvar '%s'(int) to %d", dvar.debugName_, dvar.int_ );
            break;
        case dvar_t::Float:
            dvar.float_ = float( atof( str ) );
            DVAR_PRINTF( "change dvar '%s'(float) to %f", dvar.debugName_, dvar.float_ );
            break;
        case dvar_t::String:
            SafeCopyDvarString( dvar, str );
            DVAR_PRINTF( "change dvar '%s'(string) to \"%s\"", dvar.debugName_, dvar.str_ );
            break;
        case dvar_t::Vec2:
            n = sscanf( str, "%f,%f", &v.x, &v.y );
            core_assert( n == 2 );
            DVAR_PRINTF( "change dvar '%s'(vec2) to { %f, %f } ", dvar.debugName_, v.x, v.y );
            break;
        case dvar_t::Vec3:
            n = sscanf( str, "%f,%f,%f", &v.x, &v.y, &v.z );
            core_assert( n == 3 );
            DVAR_PRINTF( "change dvar '%s'(vec3) to { %f, %f, %f } ", dvar.debugName_, v.x, v.y, v.z );
            break;
        case dvar_t::Vec4:
            n = sscanf( str, "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w );
            core_assert( n == 4 );
            DVAR_PRINTF( "change dvar '%s'(vec4) to { %f, %f, %f, %f } ", dvar.debugName_, v.x, v.y, v.z, v.w );
            break;
        default:
            panic( "attempt to set unknown dvar type %d to %s", static_cast<int>( dvar.type_ ), str );
            break;
    }

    return DvarError::Ok;
}

DvarError Dvar_SetIntByName_Internal( const char* name, int value )
{
    dvar_t* dvar = Dvar_FindByName_Internal( name );
    core_assert( dvar );
    if ( !dvar )
    {
        return DvarError::NotExisted;
    }

    return Dvar_SetInt_Internal( *dvar, value );
}

DvarError Dvar_SetFloatByName_Internal( const char* name, float value )
{
    dvar_t* dvar = Dvar_FindByName_Internal( name );
    core_assert( dvar );
    if ( !dvar )
    {
        return DvarError::NotExisted;
    }

    return Dvar_SetFloat_Internal( *dvar, value );
}

DvarError Dvar_SetStringByName_Internal( const char* name, const char* value )
{
    dvar_t* dvar = Dvar_FindByName_Internal( name );
    core_assert( dvar );
    if ( !dvar )
    {
        return DvarError::NotExisted;
    }

    return Dvar_SetString_Internal( *dvar, value );
}
