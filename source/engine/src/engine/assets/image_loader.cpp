#include "image_loader.h"

#include "stb_image.h"

namespace vct {

std::shared_ptr<Image> load_image(const std::string& path) {

    int width = 0;
    int height = 0;
    int num_channels = 0;

    uint8_t* data = stbi_load(path.c_str(), &width, &height, &num_channels, 0);
    DEV_ASSERT(data);

    std::vector<uint8_t> buffer;

    buffer.resize(width * height * num_channels);
    memcpy(buffer.data(), data, buffer.size());

    stbi_image_free(data);

    PixelFormat format;
    switch (num_channels) {
        case 1:
            format = FORMAT_R8_UINT;
            break;
        case 2:
            format = FORMAT_R8G8_UINT;
            break;
        case 3:
            format = FORMAT_R8G8B8_UINT;
            break;
        case 4:
            format = FORMAT_R8G8B8A8_UINT;
            break;
        default:
            CRASH_NOW();
            break;
    }

    return std::make_shared<Image>(format, width, height, num_channels, buffer);
}

}  // namespace vct