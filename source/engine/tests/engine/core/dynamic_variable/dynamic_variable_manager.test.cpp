#include "core/dynamic_variable/dynamic_variable_manager.h"

#include "core/os/os.h"

namespace vct {

#include "test_dvars.h"

extern void register_test_dvars();

using Commands = std::vector<std::string>;

TEST(DynamicVariableParser, invalid_command) {
    register_test_dvars();

    Commands commands = { "+abc" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_FALSE(ok);
    EXPECT_EQ(parser.get_error(), "unknown command '+abc'");
}

TEST(DynamicVariableParser, invalid_dvar_name) {
    register_test_dvars();

    Commands commands = { "+set", "test_int1" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_FALSE(ok);
    EXPECT_EQ(parser.get_error(), "dvar 'test_int1' not found");
}

TEST(DynamicVariableParser, unexpected_eof) {
    register_test_dvars();

    Commands commands = { "+set", "test_int" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_FALSE(ok);
    EXPECT_EQ(parser.get_error(), "invalid arguments: +set test_int");
}

TEST(DynamicVariableParser, set_int) {
    register_test_dvars();

    Commands commands = { "+set", "test_int", "1001" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_INT(test_int), 1001);
}

TEST(DynamicVariableParser, set_float) {
    register_test_dvars();

    Commands commands = { "+set", "test_float", "1001.1" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_FLOAT(test_float), 1001.1f);
}

TEST(DynamicVariableParser, set_string) {
    register_test_dvars();

    Commands commands = { "+set", "test_string", "1001.1" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_STRING(test_string), "1001.1");
}

TEST(DynamicVariableParser, set_vec2) {
    register_test_dvars();

    Commands commands = { "+set", "test_vec2", "6", "7" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_VEC2(test_vec2), vec2(6, 7));
}

TEST(DynamicVariableParser, set_vec3) {
    register_test_dvars();

    Commands commands = { "+set", "test_vec3", "6", "7", "8" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_VEC3(test_vec3), vec3(6, 7, 8));
}

TEST(DynamicVariableParser, set_vec4) {
    register_test_dvars();

    Commands commands = { "+set", "test_vec4", "6", "7", "8", "9" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_VEC4(test_vec4), vec4(6, 7, 8, 9));
}

TEST(DynamicVariableParser, set_ivec2) {
    register_test_dvars();

    Commands commands = { "+set", "test_ivec2", "6", "7" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_IVEC2(test_ivec2), ivec2(6, 7));
}

TEST(DynamicVariableParser, set_ivec3) {
    register_test_dvars();

    Commands commands = { "+set", "test_ivec3", "6", "7", "8" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_IVEC3(test_ivec3), ivec3(6, 7, 8));
}

TEST(DynamicVariableParser, set_ivec4) {
    register_test_dvars();

    Commands commands = { "+set", "test_ivec4", "6", "7", "8", "9" };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_IVEC4(test_ivec4), ivec4(6, 7, 8, 9));
}

TEST(DynamicVariableParser, multiple_set_success) {
    register_test_dvars();

    Commands commands = {
        // clang-format off
        "+set", "test_ivec4", "7", "8", "9", "10",
        "+set", "test_int", "1002",
        // clang-format on
    };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_TRUE(ok);
    EXPECT_EQ(DVAR_GET_IVEC4(test_ivec4), ivec4(7, 8, 9, 10));
    EXPECT_EQ(DVAR_GET_INT(test_int), 1002);
}

TEST(DynamicVariableParser, multiple_set_fail) {
    register_test_dvars();

    Commands commands = {
        // clang-format off
        "+set", "test_ivec4", "7", "8", "9", "10",
        "+set", "test_int", "1002",
        "+set", "test_vec4", "1",
        // clang-format on
    };

    DynamicVariableParser parser{ commands, DynamicVariableParser::SOURCE_NONE };
    bool ok = parser.parse();
    EXPECT_FALSE(ok);
    EXPECT_EQ(parser.get_error(), "invalid arguments: +set test_vec4 1");
}

}  // namespace vct
