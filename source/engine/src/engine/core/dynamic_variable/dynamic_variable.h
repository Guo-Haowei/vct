#pragma once
#include "core/math/geomath.h"

enum DvarFlags {
    DVAR_FLAG_NONE = 0,
    DVAR_FLAG_SERIALIZE = 1,
};

namespace vct {

enum VariantType {
    VARIANT_TYPE_INVALID = 0,
    VARIANT_TYPE_INT,
    VARIANT_TYPE_FLOAT,
    VARIANT_TYPE_STRING,
    VARIANT_TYPE_VEC2,
    VARIANT_TYPE_VEC3,
    VARIANT_TYPE_VEC4,
    VARIANT_TYPE_IVEC2,
    VARIANT_TYPE_IVEC3,
    VARIANT_TYPE_IVEC4,
    VARIANT_TYPE_MAX,
};

class DynamicVariable {
public:
    DynamicVariable(VariantType type, uint32_t flags, const char* desc)
        : m_type(type), m_desc(desc), m_flags(flags), m_int(0) {}

    void register_int(std::string_view key, int value);
    void register_float(std::string_view key, float value);
    void register_string(std::string_view key, std::string_view value);
    void register_vec2(std::string_view key, float x, float y);
    void register_vec3(std::string_view key, float x, float y, float z);
    void register_vec4(std::string_view key, float x, float y, float z, float w);
    void register_ivec2(std::string_view key, int x, int y);
    void register_ivec3(std::string_view key, int x, int y, int z);
    void register_ivec4(std::string_view key, int x, int y, int z, int w);

    int as_int() const;
    float as_float() const;
    const std::string& as_string() const;
    vec2 as_vec2() const;
    vec3 as_vec3() const;
    vec4 as_vec4() const;
    ivec2 as_ivec2() const;
    ivec3 as_ivec3() const;
    ivec4 as_ivec4() const;
    void* as_pointer();

    bool set_int(int value);
    bool set_float(float value);
    bool set_string(const std::string& value);
    bool set_string(std::string_view value);
    bool set_vec2(float x, float y);
    bool set_vec3(float x, float y, float z);
    bool set_vec4(float x, float y, float z, float w);
    bool set_ivec2(int x, int y);
    bool set_ivec3(int x, int y, int z);
    bool set_ivec4(int x, int y, int z, int w);

    void set_flag(uint32_t flag) { m_flags |= flag; }
    void unset_flag(uint32_t flag) { m_flags &= ~flag; }

    std::string value_to_string() const;
    void print_value_change(std::string_view source) const;

    VariantType get_type() const { return m_type; }
    const char* get_desc() const { return m_desc; }

    static DynamicVariable* find_dvar(const std::string& name);
    static void register_dvar(std::string_view key, DynamicVariable* dvar);

private:
    const VariantType m_type;
    const char* m_desc;
    uint32_t m_flags;

    union {
        int m_int;
        float m_float;
        struct {
            float x, y, z, w;
        } m_vec;
        struct {
            int x, y, z, w;
        } m_ivec;
    };
    std::string m_string;
    std::string m_name;

    inline static std::unordered_map<std::string, DynamicVariable*> s_map;
    friend class DynamicVariableManager;
};

}  // namespace vct

#define DVAR_GET_BOOL(name)    (!!(DVAR_##name).as_int())
#define DVAR_GET_INT(name)     (DVAR_##name).as_int()
#define DVAR_GET_FLOAT(name)   (DVAR_##name).as_float()
#define DVAR_GET_STRING(name)  (DVAR_##name).as_string()
#define DVAR_GET_VEC2(name)    (DVAR_##name).as_vec2()
#define DVAR_GET_VEC3(name)    (DVAR_##name).as_vec3()
#define DVAR_GET_VEC4(name)    (DVAR_##name).as_vec4()
#define DVAR_GET_IVEC2(name)   (DVAR_##name).as_ivec2()
#define DVAR_GET_IVEC3(name)   (DVAR_##name).as_ivec3()
#define DVAR_GET_IVEC4(name)   (DVAR_##name).as_ivec4()
#define DVAR_GET_POINTER(name) (DVAR_##name).as_pointer()

#define DVAR_SET_BOOL(name, value)       (DVAR_##name).set_int(!!(value))
#define DVAR_SET_INT(name, value)        (DVAR_##name).set_int(value)
#define DVAR_SET_FLOAT(name, value)      (DVAR_##name).set_float(value)
#define DVAR_SET_STRING(name, value)     (DVAR_##name).set_string(value)
#define DVAR_SET_VEC2(name, x, y)        (DVAR_##name).set_vec2(x, y)
#define DVAR_SET_VEC3(name, x, y, z)     (DVAR_##name).set_vec3(x, y, z)
#define DVAR_SET_VEC4(name, x, y, z, w)  (DVAR_##name).set_vec4(x, y, z, w)
#define DVAR_SET_IVEC2(name, x, y)       (DVAR_##name).set_ivec2(x, y)
#define DVAR_SET_IVEC3(name, x, y, z)    (DVAR_##name).set_ivec3(x, y, z)
#define DVAR_SET_IVEC4(name, x, y, z, w) (DVAR_##name).set_ivec4(x, y, z, w)
