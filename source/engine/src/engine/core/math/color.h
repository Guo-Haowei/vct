#pragma once

namespace vct {

enum ColorCode : uint32_t {
    COLOR_YELLOW = 0xE0E080,
    COLOR_RED = 0xE08080,
    COLOR_PALEGREEN = 0x98FB98,
    COLOR_SILVER = 0xA0A0A0,
    COLOR_WHITE = 0xE0E0E0,
};

struct Color {
#pragma warning(push)
#pragma warning(disable : 4201)
    union {
        struct
        {
            float r, g, b, a;
        };
        float components[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    };
#pragma warning(pop)

    constexpr Color() = default;
    constexpr Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    constexpr Color(float r, float g, float b) : Color(r, g, b, 1.0f) {}

    uint32_t to_rgb() const;
    uint32_t to_rgba() const;

    static Color hex(uint32_t hex);
    static Color hex_rgba(uint32_t hex);
};

}  // namespace vct
