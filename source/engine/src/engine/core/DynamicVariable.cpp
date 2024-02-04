#include "DynamicVariable.h"

static std::unordered_map<std::string, DynamicVariable*> sDVARMap;

void DynamicVariable::RegisterInt(const char* key, int value) {
    mType = VariantType::VARIANT_TYPE_INT;
    mIntegerValue = value;
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterFloat(const char* key, float value) {
    mType = VariantType::VARIANT_TYPE_FLOAT;
    mFloatValue = value;
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterString(const char* key, const char* value) {
    mType = VariantType::VARIANT_TYPE_STRING;
    mStringValue = value;
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterVec2(const char* key, float x, float y) {
    mType = VariantType::VARIANT_TYPE_VEC2;
    SetVec2(x, y);
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterVec3(const char* key, float x, float y, float z) {
    mType = VariantType::VARIENT_TYPE_VEC3;
    SetVec3(x, y, z);
    DynamicVariableManager::Register(key, this);
}

void DynamicVariable::RegisterVec4(const char* key, float x, float y, float z, float w) {
    mType = VariantType::VARIANT_TYPE_VEC4;
    SetVec4(x, y, z, w);
    DynamicVariableManager::Register(key, this);
}

int DynamicVariable::AsInt() const {
    DEV_ASSERT(mType == VariantType::VARIANT_TYPE_INT);
    return mIntegerValue;
}

float DynamicVariable::AsFloat() const {
    DEV_ASSERT(mType == VariantType::VARIANT_TYPE_FLOAT);
    return mFloatValue;
}

const char* DynamicVariable::AsString() const {
    DEV_ASSERT(mType == VariantType::VARIANT_TYPE_STRING);
    return mStringValue.c_str();
}

vec2 DynamicVariable::AsVec2() const {
    DEV_ASSERT(mType == VariantType::VARIANT_TYPE_VEC2);
    return vec2(mVecValue.x, mVecValue.y);
}

vec3 DynamicVariable::AsVec3() const {
    DEV_ASSERT(mType == VariantType::VARIENT_TYPE_VEC3);
    return vec3(mVecValue.x, mVecValue.y, mVecValue.z);
}

vec4 DynamicVariable::AsVec4() const {
    DEV_ASSERT(mType == VariantType::VARIANT_TYPE_VEC4);
    return vec4(mVecValue.x, mVecValue.y, mVecValue.z, mVecValue.w);
}

void* DynamicVariable::AsPointer() {
    switch (mType) {
        case VariantType::VARIANT_TYPE_INT:
        case VariantType::VARIANT_TYPE_FLOAT:
        case VariantType::VARIANT_TYPE_VEC2:
        case VariantType::VARIENT_TYPE_VEC3:
        case VariantType::VARIANT_TYPE_VEC4:
            return &mIntegerValue;
        default:
            CRASH_NOW();
            return nullptr;
    }
}

EDvarError DynamicVariable::SetInt(int value) {
    if (mType != VariantType::VARIANT_TYPE_INT) {
        return EDvarError::TypeMismatch;
    }

    mIntegerValue = value;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetFloat(float value) {
    if (mType != VariantType::VARIANT_TYPE_FLOAT) {
        return EDvarError::TypeMismatch;
    }

    mFloatValue = value;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetString(const char* value) {
    if (mType != VariantType::VARIANT_TYPE_STRING) {
        return EDvarError::TypeMismatch;
    }

    mStringValue = value;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetVec2(float x, float y) {
    if (mType != VariantType::VARIANT_TYPE_VEC2) {
        return EDvarError::TypeMismatch;
    }

    mVecValue.x = x;
    mVecValue.y = y;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetVec3(float x, float y, float z) {
    if (mType != VariantType::VARIENT_TYPE_VEC3) {
        return EDvarError::TypeMismatch;
    }

    mVecValue.x = x;
    mVecValue.y = y;
    mVecValue.z = z;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetVec4(float x, float y, float z, float w) {
    if (mType != VariantType::VARIANT_TYPE_VEC4) {
        return EDvarError::TypeMismatch;
    }

    mVecValue.x = x;
    mVecValue.y = y;
    mVecValue.z = z;
    mVecValue.w = w;
    return EDvarError::Ok;
}

EDvarError DynamicVariable::SetFromSourceString(const char* str) {
    switch (mType) {
        case VariantType::VARIANT_TYPE_INT:
            mIntegerValue = atoi(str);
            LOG_VERBOSE("change dvar '{}'(int) to {}", mDebugName.c_str(), mIntegerValue);
            break;
        case VariantType::VARIANT_TYPE_FLOAT:
            mFloatValue = float(atof(str));
            LOG_VERBOSE("change dvar '{}'(float) to {}", mDebugName.c_str(), mFloatValue);
            break;
        case VariantType::VARIANT_TYPE_STRING:
            mStringValue = str;
            LOG_VERBOSE("change dvar '{}'(string) to \"{}\"", mDebugName.c_str(), mStringValue.c_str());
            break;
        default:
            LOG_FATAL("Unknown dvar type {}", static_cast<int>(mType));
            return EDvarError::NotExisted;
    }

    return EDvarError::Ok;
}

DynamicVariable* DynamicVariableManager::Find(const char* name) {
    auto it = sDVARMap.find(name);
    if (it == sDVARMap.end()) {
        return nullptr;
    }
    return it->second;
}

void DynamicVariableManager::Register(const char* key, DynamicVariable* dvar) {
    const std::string keyStr(key);
    auto it = sDVARMap.find(keyStr);
    if (it != sDVARMap.end()) {
        LOG_ERROR("duplicated dvar {} detected", key);
    }

    dvar->mDebugName = key;

    sDVARMap.insert(std::make_pair(keyStr, dvar));
    switch (dvar->mType) {
        case VariantType::VARIANT_TYPE_INT:
            LOG_VERBOSE("register dvar '{}'(int) {}", key, dvar->mIntegerValue);
            break;
        case VariantType::VARIANT_TYPE_FLOAT:
            LOG_VERBOSE("register dvar '{}'(float) {}", key, dvar->mFloatValue);
            break;
        case VariantType::VARIANT_TYPE_STRING:
            LOG_VERBOSE("register dvar '{}'(string) \"{}\"", key, dvar->mStringValue.c_str());
            break;
        case VariantType::VARIANT_TYPE_VEC2:
            LOG_VERBOSE("register dvar '{}'(vec2) {} {}", key, dvar->mVecValue.x, dvar->mVecValue.y);
            break;
        case VariantType::VARIENT_TYPE_VEC3:
            LOG_VERBOSE("register dvar '{}'(vec3) {} {} {}", key, dvar->mVecValue.x, dvar->mVecValue.y,
                        dvar->mVecValue.z);
            break;
        case VariantType::VARIANT_TYPE_VEC4:
            LOG_VERBOSE("register dvar '{}'(vec4) {} {} {} {}", key, dvar->mVecValue.x, dvar->mVecValue.y,
                        dvar->mVecValue.z, dvar->mVecValue.w);
            break;
        default:
            LOG_FATAL("Unknown dvar type {}", static_cast<int>(dvar->mType));
            break;
    }
}
