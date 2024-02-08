#include "asset_loader.h"

#include "assets/image_loader.h"
#include "assets/scene_loader_assimp.h"
#include "assets/scene_loader_tinygltf.h"
#include "core/collections/thread_safe_ring_buffer.h"
#include "core/dynamic_variable/common_dvars.h"
#include "core/os/threads.h"
#include "core/utility/timer.h"
#include "scene/scene.h"

extern auto load_scene_assimp(const std::string& asset_path, void* data) -> std::expected<void, std::string>;
extern auto load_scene_tinygltf(const std::string& asset_path, void* data) -> std::expected<void, std::string>;

namespace vct::asset_loader {

using LoadFunc = std::expected<void, std::string> (*)(const std::string& asset_path, void* asset);

struct LoadTask {
    // @TODO: better string
    std::string asset_path;
    LoadSuccessFunc on_load_success;
};

static struct
{
    // @TODO: better wake up
    std::condition_variable wake_condition;
    std::mutex wake_mutex;
    // @TODO: better thread safe queue
    collection::ThreadSafeRingBuffer<LoadTask, 128> job_queue;

    // loader
    std::map<std::string, LoadFunc> loaders;

    // image
    std::map<std::string, std::shared_ptr<Image>> image_cache;
    std::mutex image_cache_lock;
} s_glob;

bool initialize() {
    s_glob.loaders[".gltf"] = load_scene_tinygltf;
    s_glob.loaders[".obj"] = load_scene_assimp;
    if (DVAR_GET_BOOL(force_assimp_loader)) {
        s_glob.loaders[".gltf"] = load_scene_assimp;
    }
    return true;
}

void finalize() {
    s_glob.wake_condition.notify_all();
}

// @TODO: refactor
void load_scene_async(const std::string& path, LoadSuccessFunc on_load_success) {
    s_glob.job_queue.push_back({ path, on_load_success });
    s_glob.wake_condition.notify_all();
}

std::shared_ptr<Image> find_image(const std::string& path) {
    std::lock_guard guard(s_glob.image_cache_lock);

    auto found = s_glob.image_cache.find(path);
    if (found != s_glob.image_cache.end()) {
        return found->second;
    }

    return nullptr;
}

std::shared_ptr<Image> load_image_sync(const std::string& path) {
    std::lock_guard guard(s_glob.image_cache_lock);

    auto found = s_glob.image_cache.find(path);
    if (found != s_glob.image_cache.end()) {
        LOG_VERBOSE("image {} found in cache", path);
        return found->second;
    }

    // LOG_VERBOSE("image {} not found in cache, loading...", path);
    auto ret = load_image(path);
    s_glob.image_cache[path] = ret;
    return ret;
}

static void load_scene_internal(LoadTask& task) {
    LOG("[asset_loader] Loading scene '{}'...", task.asset_path);

    std::string ext = std::filesystem::path(task.asset_path).extension().string();
    auto it = s_glob.loaders.find(ext);
    if (it == s_glob.loaders.end()) {
        LOG_FATAL("[asset_loader] Error: no suitable loader found for '{}'", task.asset_path);
        // @TODO: error callback
        return;
    }

    Timer timer;

    Scene* scene = new Scene;
    auto res = it->second(task.asset_path, scene);
    if (!res) {
        LOG_FATAL("{}", res.error());
        // @TODO: on_load_error
    } else {
        LOG("[asset_loader] Scene '{}' loaded in {}", task.asset_path, timer.get_duration_string());
        task.on_load_success(scene);
    }
}

static bool work() {
    LoadTask task;
    if (!s_glob.job_queue.pop_front(task)) {
        return false;
    }

    // @TODO: load scene
    load_scene_internal(task);

    return true;
}

void worker_main() {
    for (;;) {
        if (thread::is_shutdown_requested()) {
            break;
        }

        if (!work()) {
            std::unique_lock<std::mutex> lock(s_glob.wake_mutex);
            s_glob.wake_condition.wait(lock);
        }
    }
}

}  // namespace vct::asset_loader
