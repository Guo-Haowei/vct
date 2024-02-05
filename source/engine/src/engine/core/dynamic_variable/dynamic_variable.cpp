#include "dynamic_variable.h"

namespace vct {

void DynamicVariable::register_int(std::string_view key, int value) {
    m_int = value;
    register_dvar(key, this);
}

void DynamicVariable::register_float(std::string_view key, float value) {
    m_float = value;
    register_dvar(key, this);
}

void DynamicVariable::register_string(std::string_view key, std::string_view value) {
    m_string = value;
    register_dvar(key, this);
}

void DynamicVariable::register_vec2(std::string_view key, float x, float y) {
    m_vec.x = x;
    m_vec.y = y;
    register_dvar(key, this);
}

void DynamicVariable::register_vec3(std::string_view key, float x, float y, float z) {
    m_vec.x = x;
    m_vec.y = y;
    m_vec.z = z;
    register_dvar(key, this);
}

void DynamicVariable::register_vec4(std::string_view key, float x, float y, float z, float w) {
    m_vec.x = x;
    m_vec.y = y;
    m_vec.z = z;
    m_vec.w = w;
    register_dvar(key, this);
}

int DynamicVariable::as_int() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_INT);
    return m_int;
}

float DynamicVariable::as_float() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_FLOAT);
    return m_float;
}

std::string_view DynamicVariable::as_string() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_STRING);
    return m_string;
}

vec2 DynamicVariable::as_vec2() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_VEC2);
    return vec2(m_vec.x, m_vec.y);
}

vec3 DynamicVariable::as_vec3() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_VEC3);
    return vec3(m_vec.x, m_vec.y, m_vec.z);
}

vec4 DynamicVariable::as_vec4() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_VEC4);
    return vec4(m_vec.x, m_vec.y, m_vec.z, m_vec.w);
}

void* DynamicVariable::as_pointer() {
    switch (m_type) {
        case VARIANT_TYPE_INT:
        case VARIANT_TYPE_FLOAT:
        case VARIANT_TYPE_VEC2:
        case VARIANT_TYPE_VEC3:
        case VARIANT_TYPE_VEC4:
            return &m_int;
        default:
            CRASH_NOW();
            return nullptr;
    }
}

bool DynamicVariable::set_int(int value) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_INT, false);

    m_int = value;
    LOG_VERBOSE("change dvar '{}'(int) to {}", m_debug_name, m_int);
    return true;
}

bool DynamicVariable::set_float(float value) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_FLOAT, false);

    m_float = value;
    LOG_VERBOSE("change dvar '{}'(float) to {}", m_debug_name, m_float);
    return true;
}

bool DynamicVariable::set_string(std::string_view value) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_STRING, false);

    m_string = value;
    LOG_VERBOSE("change dvar '{}'(string) to \"{}\"", m_debug_name, m_string);
    return true;
}

bool DynamicVariable::set_vec2(float x, float y) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_VEC2, false);

    m_vec.x = x;
    m_vec.y = y;
    LOG_VERBOSE("change dvar '{}'(vec2) to {},{}", m_debug_name, m_vec.x, m_vec.y);
    return true;
}

bool DynamicVariable::set_vec3(float x, float y, float z) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_VEC3, false);

    m_vec.x = x;
    m_vec.y = y;
    m_vec.z = z;
    LOG_VERBOSE("change dvar '{}'(vec3) to {},{},{}", m_debug_name, m_vec.x, m_vec.y, m_vec.z);
    return true;
}

bool DynamicVariable::set_vec4(float x, float y, float z, float w) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_VEC4, false);

    m_vec.x = x;
    m_vec.y = y;
    m_vec.z = z;
    m_vec.w = w;
    LOG_VERBOSE("change dvar '{}'(vec4) to {},{},{},{}", m_debug_name, m_vec.x, m_vec.y, m_vec.z, m_vec.w);
    return true;
}

DynamicVariable* DynamicVariable::find_dvar(std::string_view name) {
    auto it = s_map.find(std::string(name));
    if (it == s_map.end()) {
        return nullptr;
    }
    return it->second;
}

void DynamicVariable::register_dvar(std::string_view key, DynamicVariable* dvar) {
    const std::string keyStr(key);
    auto it = s_map.find(keyStr);
    if (it != s_map.end()) {
        LOG_ERROR("duplicated dvar {} detected", key);
    }

    dvar->m_debug_name = key;

    s_map.insert(std::make_pair(keyStr, dvar));
}

}  // namespace vct
