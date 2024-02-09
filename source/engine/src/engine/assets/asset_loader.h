#pragma once
#include "image.h"

namespace vct {
class Scene;
}  // namespace vct

namespace vct {

struct Text {
    std::string buffer;
};

}  // namespace vct

namespace vct::asset_loader {

using LoadSuccessFunc = void (*)(void*);

bool initialize();
void finalize();

void load_scene_async(const std::string& path, LoadSuccessFunc on_success);

std::shared_ptr<Text> load_file_sync(const std::string& path);

std::shared_ptr<Image> load_image_sync(const std::string& path);
std::shared_ptr<Image> find_image(const std::string& path);

void worker_main();

}  // namespace vct::asset_loader
