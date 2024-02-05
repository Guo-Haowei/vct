#include "core/math/color.h"

namespace vct::math {

TEST(Color, to_rgb) {
    auto c = Color::hex(ColorCode::COLOR_RED);
    uint32_t rgb = c.to_rgb();
    EXPECT_EQ(rgb, ColorCode::COLOR_RED);
}

}  // namespace vct::math
