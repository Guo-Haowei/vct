#pragma once
#include "core/math/geomath.h"

namespace vct {

enum VariantType {
    VARIANT_TYPE_INVALID,
    VARIANT_TYPE_INT,
    VARIANT_TYPE_FLOAT,
    VARIANT_TYPE_STRING,
    VARIANT_TYPE_VEC2,
    VARIANT_TYPE_VEC3,
    VARIANT_TYPE_VEC4,
};

enum DvarFlags {
    DVAR_FLAG_NONE = 0,
    DVAR_FLAG_SERIALIZE = 1,
};

class DynamicVariable {
public:
    DynamicVariable(VariantType type, uint32_t flags)
        : m_type(type), m_flags(flags), m_int(0) {}

    void register_int(std::string_view key, int value);
    void register_float(std::string_view key, float value);
    void register_string(std::string_view key, std::string_view value);
    void register_vec2(std::string_view key, float x, float y);
    void register_vec3(std::string_view key, float x, float y, float z);
    void register_vec4(std::string_view key, float x, float y, float z, float w);

    int as_int() const;
    float as_float() const;
    std::string_view as_string() const;
    vec2 as_vec2() const;
    vec3 as_vec3() const;
    vec4 as_vec4() const;
    void* as_pointer();

    bool set_int(int value);
    bool set_float(float value);
    bool set_string(std::string_view value);
    bool set_vec2(float x, float y);
    bool set_vec3(float x, float y, float z);
    bool set_vec4(float x, float y, float z, float w);

    VariantType get_type() const { return m_type; }

    static DynamicVariable* find_dvar(std::string_view name);
    static void register_dvar(std::string_view key, DynamicVariable* dvar);

private:
    const VariantType m_type;
    const uint32_t m_flags;

    union {
        int m_int;
        float m_float;
        struct {
            float x, y, z, w;
        } m_vec;
    };
    std::string m_string;
    std::string m_debug_name;

    inline static std::unordered_map<std::string, DynamicVariable*> s_map;
};

}  // namespace vct

#define DVAR_GET_INT(name)     (DVAR_##name).as_int()
#define DVAR_GET_BOOL(name)    (!!(DVAR_##name).as_int())
#define DVAR_GET_FLOAT(name)   (DVAR_##name).as_float()
#define DVAR_GET_STRING(name)  (DVAR_##name).as_string()
#define DVAR_GET_VEC2(name)    (DVAR_##name).as_vec2()
#define DVAR_GET_VEC3(name)    (DVAR_##name).as_vec3()
#define DVAR_GET_VEC4(name)    (DVAR_##name).as_vec4()
#define DVAR_GET_POINTER(name) (DVAR_##name).as_pointer()
