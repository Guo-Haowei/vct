#pragma once
#include "Math/GeoMath.h"

enum class EDvarType
{
    Invalid,
    Integer,
    Float,
    String,
    Vec2,
    Vec3,
    Vec4,
};

enum class EDvarError
{
    Ok,
    NotExisted,
    TypeMismatch,
};

class DynamicVariable
{
public:
    void RegisterInt(const char* key, int value);
    void RegisterFloat(const char* key, float value);
    void RegisterString(const char* key, const char* value);
    void RegisterVec2(const char* key, float x, float y);
    void RegisterVec3(const char* key, float x, float y, float z);
    void RegisterVec4(const char* key, float x, float y, float z, float w);

    int AsInt() const;
    float AsFloat() const;
    const char* AsString() const;
    vec2 AsVec2() const;
    vec3 AsVec3() const;
    vec4 AsVec4() const;
    void* AsPointer();

    EDvarError SetInt(int value);
    EDvarError SetFloat(float value);
    EDvarError SetString(const char* value);
    EDvarError SetVec2(float x, float y);
    EDvarError SetVec3(float x, float y, float z);
    EDvarError SetVec4(float x, float y, float z, float w);

    EDvarError SetFromSourceString(const char* str);

private:
    EDvarType mType;

    union
    {
        int mIntegerValue;
        float mFloatValue;
        struct
        {
            float x, y, z, w;
        } mVecValue;
    };
    std::string mStringValue;
    std::string mDebugName;

    friend class DynamicVariableManager;
};

class DynamicVariableManager
{
public:
    static DynamicVariable* Find(const char* name);
    static void Register(const char* key, DynamicVariable* dvar);
};

#define DVAR_GET_INT(name)     (DVAR_##name).AsInt()
#define DVAR_GET_BOOL(name)    (!!(DVAR_##name).AsInt())
#define DVAR_GET_FLOAT(name)   (DVAR_##name).AsFloat()
#define DVAR_GET_STRING(name)  (DVAR_##name).AsString()
#define DVAR_GET_VEC2(name)    (DVAR_##name).AsVec2()
#define DVAR_GET_VEC3(name)    (DVAR_##name).AsVec3()
#define DVAR_GET_VEC4(name)    (DVAR_##name).AsVec4()
#define DVAR_GET_POINTER(name) (DVAR_##name).AsPointer()

#define DVAR_SET_INT(name, value)       (DVAR_##name).SetInt(value)
#define DVAR_SET_FLOAT(name, value)     (DVAR_##name).SetFloat(value)
#define DVAR_SET_STRING(name, value)    (DVAR_##name).SetString(value)
#define DVAR_SET_VEC2(name, x, y)       Dvar_SetVec_Internal(DVAR_##name, x, y, 0.0f, 0.0f, 2)
#define DVAR_SET_VEC3(name, x, y, z)    Dvar_SetVec_Internal(DVAR_##name, x, y, z, 0.0f, 3)
#define DVAR_SET_VEC4(name, x, y, z, w) Dvar_SetVec_Internal(DVAR_##name, x, y, z, w, 4)
