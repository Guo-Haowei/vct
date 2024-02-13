#include "core/dynamic_variable/dynamic_variable.h"

#include "core/io/archive.h"

namespace vct {

#define DEFINE_DVAR
#include "test_dvars.h"

void register_test_dvars() {
    static bool s_registered = false;
    if (!s_registered) {
#define REGISTER_DVAR
#include "test_dvars.h"
    }
    s_registered = true;
}

extern void assert_handler(void*, std::string_view, std::string_view, int, std::string_view);

TEST(DynamicVariable, wrong_type) {
    register_test_dvars();

    ErrorHandler handler;
    handler.data.error_func = assert_handler;
    add_error_handler(&handler);

    EXPECT_EXIT(
        {
            DVAR_GET_INT(test_float);
        },
        testing::ExitedWithCode(99), "m_type == VARIANT_TYPE_INT");

    EXPECT_EXIT(
        {
            DVAR_GET_FLOAT(test_string);
        },
        testing::ExitedWithCode(99), "m_type == VARIANT_TYPE_FLOAT");

    EXPECT_EXIT(
        {
            DVAR_GET_STRING(test_vec2);
        },
        testing::ExitedWithCode(99), "m_type == VARIANT_TYPE_STRING");

    EXPECT_EXIT(
        {
            DVAR_GET_VEC2(test_vec3);
        },
        testing::ExitedWithCode(99), "m_type == VARIANT_TYPE_VEC2");

    EXPECT_EXIT(
        {
            DVAR_GET_VEC3(test_vec4);
        },
        testing::ExitedWithCode(99), "m_type == VARIANT_TYPE_VEC3");

    EXPECT_EXIT(
        {
            DVAR_GET_VEC4(test_ivec2);
        },
        testing::ExitedWithCode(99), "m_type == VARIANT_TYPE_VEC4");

    EXPECT_EXIT(
        {
            DVAR_GET_IVEC2(test_ivec3);
        },
        testing::ExitedWithCode(99), "m_type == VARIANT_TYPE_IVEC2");

    EXPECT_EXIT(
        {
            DVAR_GET_IVEC3(test_ivec4);
        },
        testing::ExitedWithCode(99), "m_type == VARIANT_TYPE_IVEC3");

    remove_error_handler(&handler);
}

TEST(DynamicVariable, int) {
    auto value = DVAR_GET_INT(test_int);
    EXPECT_EQ(value, 100);
    DVAR_SET_INT(test_int, 200);
    value = DVAR_GET_INT(test_int);
    EXPECT_EQ(value, 200);
}

TEST(DynamicVariable, float) {
    auto value = DVAR_GET_FLOAT(test_float);
    EXPECT_EQ(value, 2.3f);
    DVAR_SET_FLOAT(test_float, 1.2f);
    value = DVAR_GET_FLOAT(test_float);
    EXPECT_EQ(value, 1.2f);
}

TEST(DynamicVariable, string) {
    auto value = DVAR_GET_STRING(test_string);
    EXPECT_EQ(value, "abc");
    DVAR_SET_STRING(test_string, std::string_view("bcd"));
    value = DVAR_GET_STRING(test_string);
    EXPECT_EQ(value, "bcd");
}

TEST(DynamicVariable, vec2) {
    auto value = DVAR_GET_VEC2(test_vec2);
    EXPECT_EQ(value, vec2(1, 2));
    DVAR_SET_VEC2(test_vec2, 7.0f, 8.0f);
    value = DVAR_GET_VEC2(test_vec2);
    EXPECT_EQ(value, vec2(7, 8));
}

TEST(DynamicVariable, vec3) {
    auto value = DVAR_GET_VEC3(test_vec3);
    EXPECT_EQ(value, vec3(1, 2, 3));
    DVAR_SET_VEC3(test_vec3, 7.0f, 8.0f, 9.0f);
    value = DVAR_GET_VEC3(test_vec3);
    EXPECT_EQ(value, vec3(7, 8, 9));
}

TEST(DynamicVariable, vec4) {
    auto value = DVAR_GET_VEC4(test_vec4);
    EXPECT_EQ(value, vec4(1, 2, 3, 4));
    DVAR_SET_VEC4(test_vec4, 7.0f, 8.0f, 9.0f, 10.0f);
    value = DVAR_GET_VEC4(test_vec4);
    EXPECT_EQ(value, vec4(7, 8, 9, 10));
}

TEST(DynamicVariable, ivec2) {
    auto value = DVAR_GET_IVEC2(test_ivec2);
    EXPECT_EQ(value, ivec2(1, 2));
    DVAR_SET_IVEC2(test_ivec2, 7, 8);
    value = DVAR_GET_IVEC2(test_ivec2);
    EXPECT_EQ(value, ivec2(7, 8));
}

TEST(DynamicVariable, ivec3) {
    auto value = DVAR_GET_IVEC3(test_ivec3);
    EXPECT_EQ(value, ivec3(1, 2, 3));
    DVAR_SET_IVEC3(test_ivec3, 7, 8, 9);
    value = DVAR_GET_IVEC3(test_ivec3);
    EXPECT_EQ(value, ivec3(7, 8, 9));
}

TEST(DynamicVariable, ivec4) {
    auto value = DVAR_GET_IVEC4(test_ivec4);
    EXPECT_EQ(value, ivec4(1, 2, 3, 4));
    DVAR_SET_IVEC4(test_ivec4, 7, 8, 9, 10);
    value = DVAR_GET_IVEC4(test_ivec4);
    EXPECT_EQ(value, ivec4(7, 8, 9, 10));
}

}  // namespace vct
