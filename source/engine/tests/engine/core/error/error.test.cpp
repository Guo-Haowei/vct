#include "engine/core/error/error.h"

namespace vct {

TEST(error, constructor_no_string) {
    constexpr size_t LINE_NUMBER = __LINE__;
    auto err = VCT_ERROR(100).error();
    EXPECT_EQ(err.line, LINE_NUMBER + 1);
    EXPECT_EQ(err.get_value(), 100);
    EXPECT_EQ(err.get_message(), "");
}

TEST(error, constructor_with_format) {
    constexpr size_t LINE_NUMBER = __LINE__;
    auto err = VCT_ERROR(10.0f, "({}={}={})", 1, 2, 3).error();
    EXPECT_EQ(err.line, LINE_NUMBER + 1);
    EXPECT_EQ(err.get_value(), 10.0f);
    EXPECT_EQ(err.get_message(), "(1=2=3)");
}

}  // namespace vct
