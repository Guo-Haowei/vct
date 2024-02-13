#pragma once
#include "image.h"
#include "scene_importer.h"

namespace vct {
class Scene;
}  // namespace vct

// @TODO: make it asset manager, and load with multiple threads

namespace vct {

struct File {
    std::vector<char> buffer;
};

}  // namespace vct

namespace vct::asset_loader {

using ImportSuccessFunc = void (*)(void*);
using ImportErrorFunc = void (*)(const std::string& error);

bool initialize();
void finalize();

void load_scene_async(ImporterName importer, const std::string& path, ImportSuccessFunc on_success, ImportErrorFunc on_error = nullptr);

std::shared_ptr<File> load_file_sync(const std::string& path);
std::shared_ptr<File> find_file(const std::string& path);

std::shared_ptr<Image> load_image_sync(const std::string& path);
std::shared_ptr<Image> find_image(const std::string& path);

void worker_main();

}  // namespace vct::asset_loader
