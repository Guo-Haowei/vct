#include "DynamicVariable.h"

#include <string>
#include <unordered_map>

#include "Core/Check.h"

static std::unordered_map<std::string, DynamicVariable*> sDVARMap;

void DynamicVariable::RegisterInt(const char* key, int value)
{
    mType = EDvarType::Integer;
    mIntegerValue = value;
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterFloat(const char* key, float value)
{
    mType = EDvarType::Float;
    mFloatValue = value;
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterString(const char* key, const char* value)
{
    mType = EDvarType::String;
    mStringValue = value;
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterVec2(const char* key, float x, float y)
{
    mType = EDvarType::Vec2;
    SetVec2(x, y);
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterVec3(const char* key, float x, float y, float z)
{
    mType = EDvarType::Vec3;
    SetVec3(x, y, z);
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterVec4(const char* key, float x, float y, float z, float w)
{
    mType = EDvarType::Vec4;
    SetVec4(x, y, z, w);
    DynamicVariableManager::Register(key, this);
}

int DynamicVariable::AsInt() const
{
    check(mType == EDvarType::Integer);
    return mIntegerValue;
}

float DynamicVariable::AsFloat() const
{
    check(mType == EDvarType::Float);
    return mFloatValue;
}

const char* DynamicVariable::AsString() const
{
    check(mType == EDvarType::String);
    return mStringValue.c_str();
}

vec2 DynamicVariable::AsVec2() const
{
    check(mType == EDvarType::Vec2);
    return vec2(mVecValue.x, mVecValue.y);
}

vec3 DynamicVariable::AsVec3() const
{
    check(mType == EDvarType::Vec3);
    return vec3(mVecValue.x, mVecValue.y, mVecValue.z);
}

vec4 DynamicVariable::AsVec4() const
{
    check(mType == EDvarType::Vec4);
    return vec4(mVecValue.x, mVecValue.y, mVecValue.z, mVecValue.w);
}

void* DynamicVariable::AsPointer()
{
    switch (mType)
    {
        case EDvarType::Integer:
        case EDvarType::Float:
        case EDvarType::Vec2:
        case EDvarType::Vec3:
        case EDvarType::Vec4:
            return &mIntegerValue;
        default:
            unreachable();
            return nullptr;
    }
}

EDvarError DynamicVariable::SetInt(int value)
{
    if (mType != EDvarType::Integer)
    {
        return EDvarError::TypeMismatch;
    }

    mIntegerValue = value;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetFloat(float value)
{
    if (mType != EDvarType::Float)
    {
        return EDvarError::TypeMismatch;
    }

    mFloatValue = value;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetString(const char* value)
{
    if (mType != EDvarType::String)
    {
        return EDvarError::TypeMismatch;
    }

    mStringValue = value;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetVec2(float x, float y)
{
    if (mType != EDvarType::Vec2)
    {
        return EDvarError::TypeMismatch;
    }

    mVecValue.x = x;
    mVecValue.y = y;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetVec3(float x, float y, float z)
{
    if (mType != EDvarType::Vec3)
    {
        return EDvarError::TypeMismatch;
    }

    mVecValue.x = x;
    mVecValue.y = y;
    mVecValue.z = z;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetVec4(float x, float y, float z, float w)
{
    if (mType != EDvarType::Vec4)
    {
        return EDvarError::TypeMismatch;
    }

    mVecValue.x = x;
    mVecValue.y = y;
    mVecValue.z = z;
    mVecValue.w = w;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetFromSourceString(const char* str)
{
    switch (mType)
    {
        case EDvarType::Integer:
            mIntegerValue = atoi(str);
            LOG_DEBUG("change dvar '{}'(int) to {}", mDebugName.c_str(), mIntegerValue);
            break;
        case EDvarType::Float:
            mFloatValue = float(atof(str));
            LOG_DEBUG("change dvar '{}'(float) to {}", mDebugName.c_str(), mFloatValue);
            break;
        case EDvarType::String:
            mStringValue = str;
            LOG_DEBUG("change dvar '{}'(string) to \"{}\"", mDebugName.c_str(), mStringValue.c_str());
            break;
        default:
            LOG_FATAL("Unknown dvar type {}", static_cast<int>(mType));
            return EDvarError::NotExisted;
    }

    return EDvarError::Ok;
}

DynamicVariable* DynamicVariableManager::Find(const char* name)
{
    auto it = sDVARMap.find(name);
    if (it == sDVARMap.end())
    {
        return nullptr;
    }
    return it->second;
}

void DynamicVariableManager::Register(const char* key, DynamicVariable* dvar)
{
    const std::string keyStr(key);
    auto it = sDVARMap.find(keyStr);
    if (it != sDVARMap.end())
    {
        LOG_ERROR("duplicated dvar {} detected", key);
    }

    dvar->mDebugName = key;

    sDVARMap.insert(std::make_pair(keyStr, dvar));
    switch (dvar->mType)
    {
        case EDvarType::Integer:
            LOG_DEBUG("register dvar '{}'(int) {}", key, dvar->mIntegerValue);
            break;
        case EDvarType::Float:
            LOG_DEBUG("register dvar '{}'(float) {}", key, dvar->mFloatValue);
            break;
        case EDvarType::String:
            LOG_DEBUG("register dvar '{}'(string) \"{}\"", key, dvar->mStringValue.c_str());
            break;
        case EDvarType::Vec2:
            LOG_DEBUG("register dvar '{}'(vec2) {} {}", key, dvar->mVecValue.x, dvar->mVecValue.y);
            break;
        case EDvarType::Vec3:
            LOG_DEBUG("register dvar '{}'(vec3) {} {} {}", key, dvar->mVecValue.x, dvar->mVecValue.y, dvar->mVecValue.z);
            break;
        case EDvarType::Vec4:
            LOG_DEBUG("register dvar '{}'(vec4) {} {} {} {}", key, dvar->mVecValue.x, dvar->mVecValue.y, dvar->mVecValue.z, dvar->mVecValue.w);
            break;
        default:
            LOG_FATAL("Unknown dvar type {}", static_cast<int>(dvar->mType));
            break;
    }
}
