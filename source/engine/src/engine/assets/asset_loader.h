#pragma once
#include "image.h"

namespace vct {
class Scene;
}  // namespace vct

namespace vct::asset_loader {

using LoadSuccessFunc = void (*)(void*);

bool initialize();
void finalize();

void request_scene(const std::string& scene_path, LoadSuccessFunc on_success);

std::shared_ptr<Image> find_image(const std::string& image_path);

void worker_main();

}  // namespace vct::asset_loader
