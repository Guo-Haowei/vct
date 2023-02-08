#pragma once
#include <cstdint>
#include <string>

enum class PIXEL_FORMAT {
    UNKNOWN,
    R8,
    RG8,
    RGB8,
    RGBA8,
    R16,
    RG16,
    RGB16,
    RGBA16,
    R32,
    RG32,
    RGB32,
    RGBA32,
    R10G10B10A2,
    R5G6B5,
    D24R8,
    D32
};

struct Image {
    Image(const Image& rhs) = delete;
    Image& operator=(const Image& rhs) = delete;

    Image() = default;
    ~Image() { Release(); }

    void Release();

    PIXEL_FORMAT m_pixelFormat{ PIXEL_FORMAT::UNKNOWN };
    uint8_t* m_pPixels{ nullptr };
    uint32_t m_width{ 0 };
    uint32_t m_height{ 0 };

    static bool Load(const std::string& sys_path, Image& out_image );
};
