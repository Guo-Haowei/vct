#include "core/math/geomath.h"

namespace vct {

TEST(align, roundup) {
    EXPECT_EQ(align(1, 4), 4);
    EXPECT_EQ(align(2, 4), 4);
    EXPECT_EQ(align(3, 4), 4);
    EXPECT_EQ(align(5, 4), 8);
    EXPECT_EQ(align(6, 4), 8);

    EXPECT_EQ(align(6, 8), 8);
    EXPECT_EQ(align(7, 8), 8);
    EXPECT_EQ(align(9, 8), 16);
    EXPECT_EQ(align(10, 8), 16);
    EXPECT_EQ(align(11, 8), 16);
    EXPECT_EQ(align(12, 8), 16);
    EXPECT_EQ(align(13, 8), 16);
}

TEST(align, no_roundup) {
    EXPECT_EQ(align(8, 4), 8);
    EXPECT_EQ(align(8, 8), 8);
    EXPECT_EQ(align(16, 8), 16);
    EXPECT_EQ(align(128, 128), 128);
}

TEST(align, uint64) {
    EXPECT_EQ(align(7llu, 8llu), 8llu);
    EXPECT_EQ(align(1023llu, 1024llu), 1024llu);
    EXPECT_EQ(align(1023llu, 256llu), 1024llu);
}

TEST(align, compile_time) {
    static_assert(align(10, 16) == 16);
    static_assert(align(16, 16) == 16);
    static_assert(align(31, 16) == 32);
}

TEST(next_power_of_two, roundup) {
    EXPECT_EQ(next_power_of_two(3), 4);
    EXPECT_EQ(next_power_of_two(9), 16);
    EXPECT_EQ(next_power_of_two(10), 16);
    EXPECT_EQ(next_power_of_two(10), 16);
    EXPECT_EQ(next_power_of_two(17), 32);
    EXPECT_EQ(next_power_of_two(39), 64);
}

TEST(next_power_of_two, no_roundup) {
    EXPECT_EQ(next_power_of_two(4), 4);
    EXPECT_EQ(next_power_of_two(8), 8);
    EXPECT_EQ(next_power_of_two(16), 16);
    EXPECT_EQ(next_power_of_two(32), 32);
    EXPECT_EQ(next_power_of_two(64), 64);
}

TEST(next_power_of_two, compile_time) {
    static_assert(next_power_of_two(9) == 16);
    static_assert(next_power_of_two(16) == 16);
    static_assert(next_power_of_two(17) == 32);
}

}  // namespace vct
