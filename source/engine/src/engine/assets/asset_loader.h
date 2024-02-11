#pragma once
#include "image.h"

namespace vct {
class Scene;
}  // namespace vct

namespace vct {

struct File {
    std::vector<char> buffer;
};

}  // namespace vct

namespace vct::asset_loader {

using LoadSuccessFunc = void (*)(void*);

bool initialize();
void finalize();

// @TODO: allow load with different loader
// LOADER_ASSIMP
// LOADER_TINYGLTF
void load_scene_async(const std::string& path, LoadSuccessFunc on_success);

std::shared_ptr<File> load_file_sync(const std::string& path);
std::shared_ptr<File> find_file(const std::string& path);

std::shared_ptr<Image> load_image_sync(const std::string& path);
std::shared_ptr<Image> find_image(const std::string& path);

void worker_main();

}  // namespace vct::asset_loader
