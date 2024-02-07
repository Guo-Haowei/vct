#include "asset_manager.h"

#include "image_loader.h"

namespace vct {

// @TODO: refactor
static AssetManager* s_asset_manager = new AssetManager;

std::shared_ptr<Image> AssetManager::find_image(const std::string& image_path) {
    std::lock_guard guard(m_image_cache_lock);

    auto found = m_image_cache.find(image_path);
    if (found != m_image_cache.end()) {
        return found->second;
    }

    return nullptr;
}

std::shared_ptr<Image> AssetManager::request_image_sync(const std::string& image_path) {
    std::lock_guard guard(m_image_cache_lock);

    auto found = m_image_cache.find(image_path);
    if (found != m_image_cache.end()) {
        LOG_VERBOSE("image {} found in cache", image_path);
        return found->second;
    }

    LOG_VERBOSE("image {} not found in cache, loading...", image_path);
    auto ret = load_image(image_path);
    m_image_cache[image_path] = ret;
    return ret;
}

std::shared_ptr<Image> AssetManager::request_image_async(const std::string& image_path) {
    unused(image_path);
    CRASH_NOW();
    return nullptr;
}

void loader_main() {
}

}  // namespace vct
