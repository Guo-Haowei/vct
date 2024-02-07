#pragma once
#include "core/objects/singleton.h"
#include "image.h"

namespace vct {

enum AssetType {
    ASSET_TYPE_IMAGE,
    ASSET_TYPE_MAX,
};

struct LoadTask {
    std::string asset_path;
};

class AssetManager : public Singleton<AssetManager> {
public:
    std::shared_ptr<Image> find_image(const std::string& image_path);
    std::shared_ptr<Image> request_image_sync(const std::string& image_path);
    std::shared_ptr<Image> request_image_async(const std::string& image_path);

private:
    std::map<std::string, std::shared_ptr<Image>> m_image_cache;
    std::mutex m_image_cache_lock;
};

void loader_main();

}  // namespace vct
