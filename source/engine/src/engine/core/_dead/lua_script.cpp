#if 0

extern "C" {
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

#include "core/dynamic_variable/dynamic_variable.h"

#define FIND_DVAR_OR_RETURN(variable, name)                                 \
    vct::DynamicVariable* variable = vct::DynamicVariable::find_dvar(name); \
    if (!variable) {                                                        \
        LOG_ERROR("dvar '{}' not found!", name);                            \
        return 0;                                                           \
    }

#define SET_DVAR(expr)                     \
    {                                      \
        if (!(expr)) {                     \
            LOG_ERROR("{} failed", #expr); \
        }                                  \
    }

extern "C" {

// usage: Dvar.SetInt(string, integer)
static int LuaDvarFunc_SetInt(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    lua_Integer value = luaL_checkinteger(L, 2);

    FIND_DVAR_OR_RETURN(dvar, name);
    SET_DVAR(dvar->set_int(static_cast<int>(value)));
    return 0;
}

// usage: Dvar.SetFloat(string, float)
static int LuaDvarFunc_SetFloat(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const lua_Number value = luaL_checknumber(L, 2);

    FIND_DVAR_OR_RETURN(dvar, name);
    SET_DVAR(dvar->set_float(static_cast<float>(value)));
    return 0;
}

// usage: Dvar.SetString(string, string)
static int LuaDvarFunc_SetString(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const char* value = luaL_checkstring(L, 2);

    FIND_DVAR_OR_RETURN(dvar, name);
    SET_DVAR(dvar->set_string(value));
    return 0;
}

// usage: Dvar.SetVec2(string, number, number)
static int LuaDvarFunc_SetVec2(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const lua_Number x = luaL_checknumber(L, 2);
    const lua_Number y = luaL_checknumber(L, 3);

    FIND_DVAR_OR_RETURN(dvar, name);
    SET_DVAR(dvar->set_vec2(static_cast<float>(x), static_cast<float>(y)));
    return 0;
}

// usage: Dvar.SetVec3(string, number, number, number)
static int LuaDvarFunc_SetVec3(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const lua_Number x = luaL_checknumber(L, 2);
    const lua_Number y = luaL_checknumber(L, 3);
    const lua_Number z = luaL_checknumber(L, 4);

    FIND_DVAR_OR_RETURN(dvar, name);
    SET_DVAR(dvar->set_vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
    return 0;
}

// usage: Dvar.SetVec4(string, number, number, number, number)
static int LuaDvarFunc_SetVec4(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const lua_Number x = luaL_checknumber(L, 2);
    const lua_Number y = luaL_checknumber(L, 3);
    const lua_Number z = luaL_checknumber(L, 4);
    const lua_Number w = luaL_checknumber(L, 5);

    FIND_DVAR_OR_RETURN(dvar, name);
    SET_DVAR(dvar->set_vec4(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w)));
    return 0;
}

#define LUA_DVAR_LIB(func) \
    { #func, LuaDvarFunc_##func }

static const luaL_Reg s_funcs[] = { LUA_DVAR_LIB(SetInt), LUA_DVAR_LIB(SetFloat), LUA_DVAR_LIB(SetVec2), LUA_DVAR_LIB(SetVec3), LUA_DVAR_LIB(SetVec4), LUA_DVAR_LIB(SetString), { nullptr, nullptr } };

static int luaopen_EngineLib(lua_State* L) {
    luaL_newlib(L, s_funcs);
    return 1;
}
}

bool Com_ExecLua(const char* path) {
    LOG("[lua] executing {}", path);
    lua_State* L = luaL_newstate();
    DEV_ASSERT(L);
    if (L == nullptr) {
        return false;
    }

    luaL_openlibs(L);
    // Com_OpenLib( L );
    luaL_requiref(L, "Dvar", luaopen_EngineLib, 1);

    int code = luaL_dofile(L, path);
    switch (code) {
        case LUA_OK:
            break;
        default:
            lua_error(L);
            const char* err = lua_tostring(L, -1);
            LOG_ERROR("[lua] error {}\n{}", code, err);
            break;
    }

    lua_close(L);
    return code == LUA_OK;
}

#endif