#include "asset_loader.h"

#include "assets/image_loader.h"
#include "assets/scene_loader_assimp.h"
#include "core/collections/thread_safe_ring_buffer.h"
#include "core/os/threads.h"
#include "scene/scene.h"

namespace vct::asset_loader {

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

    // image
    std::map<std::string, std::shared_ptr<Image>> image_cache;
    std::mutex image_cache_lock;
} s_glob;

bool initialize() {
    return true;
}

void finalize() {
    s_glob.wake_condition.notify_all();
}

// @TODO: refactor
void request_scene(const std::string& scene_path, LoadSuccessFunc on_load_success) {
    s_glob.job_queue.push_back({ scene_path, on_load_success });
    s_glob.wake_condition.notify_all();
}

std::shared_ptr<Image> find_image(const std::string& image_path) {
    std::lock_guard guard(s_glob.image_cache_lock);

    auto found = s_glob.image_cache.find(image_path);
    if (found != s_glob.image_cache.end()) {
        return found->second;
    }

    return nullptr;
}

// @TODO: refactor
static std::shared_ptr<Image> request_image_sync(const std::string& image_path) {
    std::lock_guard guard(s_glob.image_cache_lock);

    auto found = s_glob.image_cache.find(image_path);
    if (found != s_glob.image_cache.end()) {
        LOG_VERBOSE("image {} found in cache", image_path);
        return found->second;
    }

    LOG_VERBOSE("image {} not found in cache, loading...", image_path);
    auto ret = load_image(image_path);
    s_glob.image_cache[image_path] = ret;
    return ret;
}

static void load_scene_internal(LoadTask& task) {
    Scene* scene = new Scene;

    SceneLoader loader(*scene);
    loader.LoadGLTF(task.asset_path);

    LOG("Scene '{}' loaded", task.asset_path);

    for (const MaterialComponent& material : scene->get_component_array<MaterialComponent>()) {
        for (int i = 0; i < array_length(material.mTextures); ++i) {
            const std::string& image_path = material.mTextures[i].name;
            if (!image_path.empty()) {
                request_image_sync(image_path);
            }
        }
    }

    task.on_load_success(scene);
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
