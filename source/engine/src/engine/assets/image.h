#pragma once

namespace vct {

enum PixelFormat {
    FORMAT_R8_UINT,
    FORMAT_R8G8_UINT,
    FORMAT_R8G8B8_UINT,
    FORMAT_R8G8B8A8_UINT,

    FORMAT_R16_FLOAT,
    FORMAT_R16G16_FLOAT,
    FORMAT_R16G16B16_FLOAT,
    FORMAT_R16G16B16A16_FLOAT,

    FORMAT_R32_FLOAT,
    FORMAT_R32G32_FLOAT,
    FORMAT_R32G32B32_FLOAT,
    FORMAT_R32G32B32A32_FLOAT,

    FORMAT_D32_FLOAT,
};

struct Image {
    // format
    std::vector<uint8_t> data;
    int width;
    int height;
    int num_channels;
};

}  // namespace vct
