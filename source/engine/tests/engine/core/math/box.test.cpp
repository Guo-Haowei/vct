#pragma once
#include "core/math/box.h"

namespace vct::math {

TEST(Box, constructor) {
    Box3 box{ vec3(1), vec3(10) };
    EXPECT_EQ(box.get_min(), vec3(1));
    EXPECT_EQ(box.get_max(), vec3(10));
}

TEST(Box, expand_point) {
    Box3 box;
    box.expand_point(vec3(1));

    EXPECT_EQ(box.get_min(), vec3(1));
    EXPECT_EQ(box.get_max(), vec3(1));

    box.expand_point(vec3(3));
    EXPECT_EQ(box.get_min(), vec3(1));
    EXPECT_EQ(box.get_max(), vec3(3));

    box.expand_point(vec3(-10));
    EXPECT_EQ(box.get_min(), vec3(-10));
    EXPECT_EQ(box.get_max(), vec3(3));
}

TEST(Box, union_box) {
    Box3 box1{ vec3(-10), vec3(5) };
    Box3 box2{ vec3(-5), vec3(10) };

    box1.union_box(box2);
    EXPECT_EQ(box1.get_min(), vec3(-10));
    EXPECT_EQ(box1.get_max(), vec3(10));
}

TEST(Box, intersect_box) {
    Box3 box1{ vec3(-10), vec3(5) };
    Box3 box2{ vec3(-5), vec3(10) };

    box1.intersect_box(box2);
    EXPECT_EQ(box1.get_min(), vec3(-5));
    EXPECT_EQ(box1.get_max(), vec3(5));
}

TEST(Box, center_and_size) {
    Box3 box{ vec3(-10), vec3(5) };

    EXPECT_EQ(box.center(), vec3(-2.5));
    EXPECT_EQ(box.size(), vec3(15));
}

}  // namespace vct::math
