#include "lua_script.h"

extern "C" {
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "universal/dvar_api.h"

#define CHECK_DVAR_RESULT( expr )           \
    {                                       \
        DvarError _dvarErr = ( expr );      \
        if ( _dvarErr != DvarError::Ok ) {  \
            LOG_WARN( "%s failed", #expr ); \
        }                                   \
    }

extern "C" {

// usage: Dvar.SetInt(string, integer)
static int LuaDvarFunc_SetInt( lua_State* L )
{
    const char* name = luaL_checkstring( L, 1 );
    lua_Integer value = luaL_checkinteger( L, 2 );

    dvar_t* dvar = Dvar_FindByName_Internal( name );
    if ( dvar == nullptr ) {
        LOG_WARN( "[lua] dvar '%s' not found!", name );
        return 0;
    }

    CHECK_DVAR_RESULT( Dvar_SetInt_Internal( *dvar, int( value ) ) );
    return 0;
}

// usage: Dvar.SetFloat(string, float)
static int LuaDvarFunc_SetFloat( lua_State* L )
{
    const char* name = luaL_checkstring( L, 1 );
    const lua_Number value = luaL_checknumber( L, 2 );

    dvar_t* dvar = Dvar_FindByName_Internal( name );
    if ( dvar == nullptr ) {
        LOG_WARN( "[lua] dvar '%s' not found!", name );
        return 0;
    }

    CHECK_DVAR_RESULT( Dvar_SetFloat_Internal( *dvar, float( value ) ) );
    return 0;
}

// usage: Dvar.SetVec2(string, number, number)
static int LuaDvarFunc_SetVec2( lua_State* L )
{
    const char* name = luaL_checkstring( L, 1 );
    const lua_Number value1 = luaL_checknumber( L, 2 );
    const lua_Number value2 = luaL_checknumber( L, 3 );

    dvar_t* dvar = Dvar_FindByName_Internal( name );
    if ( dvar == nullptr ) {
        LOG_WARN( "[lua] dvar '%s' not found!", name );
        return 0;
    }

    CHECK_DVAR_RESULT( Dvar_SetVec_Internal( *dvar, float( value1 ), float( value2 ), 0.0f, 0.0f ) );
    return 0;
}

// usage: Dvar.SetVec3(string, number, number, number)
static int LuaDvarFunc_SetVec3( lua_State* L )
{
    const char* name = luaL_checkstring( L, 1 );
    const lua_Number value1 = luaL_checknumber( L, 2 );
    const lua_Number value2 = luaL_checknumber( L, 3 );
    const lua_Number value3 = luaL_checknumber( L, 4 );

    dvar_t* dvar = Dvar_FindByName_Internal( name );
    if ( dvar == nullptr ) {
        LOG_WARN( "[lua] dvar '%s' not found!", name );
        return 0;
    }

    CHECK_DVAR_RESULT( Dvar_SetVec_Internal( *dvar, float( value1 ), float( value2 ), float( value3 ), 0.0f ) );
    return 0;
}

// usage: Dvar.SetVec4(string, number, number, number, number)
static int LuaDvarFunc_SetVec4( lua_State* L )
{
    const char* name = luaL_checkstring( L, 1 );
    const lua_Number value1 = luaL_checknumber( L, 2 );
    const lua_Number value2 = luaL_checknumber( L, 3 );
    const lua_Number value3 = luaL_checknumber( L, 4 );
    const lua_Number value4 = luaL_checknumber( L, 5 );

    dvar_t* dvar = Dvar_FindByName_Internal( name );
    if ( dvar == nullptr ) {
        LOG_WARN( "[lua] dvar '%s' not found!", name );
        return 0;
    }

    CHECK_DVAR_RESULT( Dvar_SetVec_Internal( *dvar, float( value1 ), float( value2 ), float( value3 ), float( value4 ) ) );
    return 0;
}

// usage: Dvar.SetString(string, string)
static int LuaDvarFunc_SetString( lua_State* L )
{
    const char* name = luaL_checkstring( L, 1 );
    const char* value = luaL_checkstring( L, 2 );

    dvar_t* dvar = Dvar_FindByName_Internal( name );
    if ( dvar == nullptr ) {
        LOG_WARN( "[lua] dvar '%s' not found!", name );
        return 0;
    }

    CHECK_DVAR_RESULT( Dvar_SetString_Internal( *dvar, value ) );
    return 0;
}

#define LUA_DVAR_LIB( func )      \
    {                             \
#func, LuaDvarFunc_##func \
    }

static const luaL_Reg s_funcs[] = {
    LUA_DVAR_LIB( SetInt ),
    LUA_DVAR_LIB( SetFloat ),
    LUA_DVAR_LIB( SetVec2 ),
    LUA_DVAR_LIB( SetVec3 ),
    LUA_DVAR_LIB( SetVec4 ),
    LUA_DVAR_LIB( SetString ),
    { nullptr, nullptr }
};

static int luaopen_EngineLib( lua_State* L )
{
    luaL_newlib( L, s_funcs );
    return 1;
}
}

bool Com_ExecLua( const char* path )
{
    LOG_INFO( "[lua] executing %s", path );
    lua_State* L = luaL_newstate();
    ASSERT( L );
    if ( L == nullptr ) {
        return false;
    }

    luaL_openlibs( L );
    // Com_OpenLib( L );
    luaL_requiref( L, "Dvar", luaopen_EngineLib, 1 );

    int code = luaL_dofile( L, path );
    switch ( code ) {
        case LUA_OK:
            break;
        default:
            lua_error( L );
            const char* err = lua_tostring( L, -1 );
            LOG_ERROR( "[lua] error %d\n%s", code, err );
            break;
    }

    lua_close( L );
    return code == LUA_OK;
}