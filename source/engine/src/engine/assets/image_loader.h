#pragma once
#include "image.h"

namespace vct {

std::shared_ptr<Image> load_image(const std::string& path);

}  // namespace vct