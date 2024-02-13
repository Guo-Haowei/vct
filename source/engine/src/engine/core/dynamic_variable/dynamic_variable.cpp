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

void DynamicVariable::register_ivec2(std::string_view key, int x, int y) {
    m_ivec.x = x;
    m_ivec.y = y;
    register_dvar(key, this);
}

void DynamicVariable::register_ivec3(std::string_view key, int x, int y, int z) {
    m_ivec.x = x;
    m_ivec.y = y;
    m_ivec.z = z;
    register_dvar(key, this);
}

void DynamicVariable::register_ivec4(std::string_view key, int x, int y, int z, int w) {
    m_ivec.x = x;
    m_ivec.y = y;
    m_ivec.z = z;
    m_ivec.w = w;
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

const std::string& DynamicVariable::as_string() const {
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

ivec2 DynamicVariable::as_ivec2() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_IVEC2);
    return ivec2(m_ivec.x, m_ivec.y);
}

ivec3 DynamicVariable::as_ivec3() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_IVEC3);
    return ivec3(m_ivec.x, m_ivec.y, m_ivec.z);
}

ivec4 DynamicVariable::as_ivec4() const {
    DEV_ASSERT(m_type == VARIANT_TYPE_IVEC4);
    return ivec4(m_ivec.x, m_ivec.y, m_ivec.z, m_ivec.w);
}

void* DynamicVariable::as_pointer() {
    switch (m_type) {
        case VARIANT_TYPE_INT:
        case VARIANT_TYPE_FLOAT:
        case VARIANT_TYPE_VEC2:
        case VARIANT_TYPE_VEC3:
        case VARIANT_TYPE_VEC4:
        case VARIANT_TYPE_IVEC2:
        case VARIANT_TYPE_IVEC3:
        case VARIANT_TYPE_IVEC4:
            return &m_int;
        default:
            CRASH_NOW();
            return nullptr;
    }
}

bool DynamicVariable::set_int(int value) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_INT, false);
    m_int = value;
    return true;
}

bool DynamicVariable::set_float(float value) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_FLOAT, false);
    m_float = value;
    return true;
}

bool DynamicVariable::set_string(const std::string& value) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_STRING, false);
    m_string = value;
    return true;
}

bool DynamicVariable::set_string(std::string_view value) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_STRING, false);
    m_string = value;
    return true;
}

bool DynamicVariable::set_vec2(float x, float y) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_VEC2, false);
    m_vec.x = x;
    m_vec.y = y;
    return true;
}

bool DynamicVariable::set_vec3(float x, float y, float z) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_VEC3, false);
    m_vec.x = x;
    m_vec.y = y;
    m_vec.z = z;
    return true;
}

bool DynamicVariable::set_vec4(float x, float y, float z, float w) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_VEC4, false);
    m_vec.x = x;
    m_vec.y = y;
    m_vec.z = z;
    m_vec.w = w;
    return true;
}

bool DynamicVariable::set_ivec2(int x, int y) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_IVEC2, false);
    m_ivec.x = x;
    m_ivec.y = y;
    return true;
}

bool DynamicVariable::set_ivec3(int x, int y, int z) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_IVEC3, false);
    m_ivec.x = x;
    m_ivec.y = y;
    m_ivec.z = z;
    return true;
}

bool DynamicVariable::set_ivec4(int x, int y, int z, int w) {
    ERR_FAIL_COND_V(m_type != VARIANT_TYPE_IVEC4, false);
    m_ivec.x = x;
    m_ivec.y = y;
    m_ivec.z = z;
    m_ivec.w = w;
    return true;
}

std::string DynamicVariable::value_to_string() const {
    switch (m_type) {
        case VARIANT_TYPE_INT:
            return std::format("{}", m_int);
        case VARIANT_TYPE_FLOAT:
            return std::format("{}", m_float);
        case VARIANT_TYPE_STRING:
            return std::format("\"{}\"", m_string);
        case VARIANT_TYPE_VEC2:
            return std::format("{} {}", m_vec.x, m_vec.y);
        case VARIANT_TYPE_IVEC2:
            return std::format("{} {}", m_ivec.x, m_ivec.y);
        case VARIANT_TYPE_VEC3:
            return std::format("{} {} {}", m_vec.x, m_vec.y, m_vec.z);
        case VARIANT_TYPE_IVEC3:
            return std::format("{} {} {}", m_ivec.x, m_ivec.y, m_ivec.z);
        case VARIANT_TYPE_VEC4:
            return std::format("{} {} {} {}", m_vec.x, m_vec.y, m_vec.z, m_vec.w);
        case VARIANT_TYPE_IVEC4:
            return std::format("{} {} {} {}", m_ivec.x, m_ivec.y, m_ivec.z, m_ivec.w);
        default:
            CRASH_NOW();
            return std::string{};
    }
}

void DynamicVariable::print_value_change(std::string_view source) const {
    static const char* s_names[] = {
        "",
        "int",
        "float",
        "string",
        "vec2",
        "vec3",
        "vec4",
        "ivec2",
        "ivec3",
        "ivec4",
    };

    static_assert(array_length(s_names) == VARIANT_TYPE_MAX);

    std::string value_string = value_to_string();
    LOG_VERBOSE("[dvar] change dvar '{}'({}) to {} (source: {})", m_name, s_names[m_type], value_string, source);
}

DynamicVariable* DynamicVariable::find_dvar(const std::string& name) {
    auto it = s_map.find(name);
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

    dvar->m_name = key;

    s_map.insert(std::make_pair(keyStr, dvar));
}

}  // namespace vct
