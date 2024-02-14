#include "asset_loader.h"

#include "assets/image_loader.h"
#include "assets/scene_importer_assimp.h"
#include "assets/scene_importer_tinygltf.h"
#include "core/base/thread_safe_ring_buffer.h"
#include "core/io/file_access.h"
#include "core/os/threads.h"
#include "core/os/timer.h"
#include "scene/scene.h"

namespace vct::asset_loader {

using LoadFunc = std::expected<void, std::string> (*)(const std::string& asset_path, void* asset);

struct LoadTask {
    ImporterName importer;
    // @TODO: better string
    std::string asset_path;
    ImportSuccessFunc on_success;
    ImportErrorFunc on_error;
};

static struct {
    // @TODO: better wake up
    std::condition_variable wake_condition;
    std::mutex wake_mutex;
    // @TODO: better thread safe queue
    ThreadSafeRingBuffer<LoadTask, 128> job_queue;

    // image
    std::map<std::string, std::shared_ptr<Image>> image_cache;
    std::mutex image_cache_lock;

    // file
    std::map<std::string, std::shared_ptr<File>> text_cache;
} s_glob;

bool initialize() {
    // @TODO: dir_access
    // @TODO: async
    // force load all shaders
#if 0
    const std::string preload[] = {
        "@res://fonts/DroidSans.ttf",
        "@res://glsl/vsinput.glsl.h",
        "@res://glsl/cbuffer.glsl.h",
        "@res://glsl/common.glsl",
        "@res://glsl/mesh_static.vert",
        "@res://glsl/mesh_animated.vert",
        "@res://glsl/depth_static.vert",
        "@res://glsl/depth_animated.vert",
        "@res://glsl/depth.frag",
        "@res://glsl/fullscreen.vert",
        "@res://glsl/fxaa.frag",
        "@res://glsl/gbuffer.frag",
        "@res://glsl/pbr.glsl",
        "@res://glsl/shadow.glsl",
        "@res://glsl/ssao.frag",
        "@res://glsl/textureCB.glsl",
        "@res://glsl/lighting_vxgi.frag",
        "@res://glsl/debug/texture.frag",
        "@res://glsl/voxel/voxelization.vert",
        "@res://glsl/voxel/voxelization.geom",
        "@res://glsl/voxel/voxelization.frag",
        "@res://glsl/voxel/post.comp",
    };

    Timer timer;
    for (int i = 0; i < array_length(preload); ++i) {
        if (load_file_sync(preload[i])) {
            LOG_VERBOSE("[asset_loader] resource '{}' preloaded", preload[i]);
        }
    }
    LOG_VERBOSE("[asset_loader] preloaded {} assets in {}", array_length(preload), timer.get_duration_string());
#endif

    return true;
}

void finalize() {
    s_glob.wake_condition.notify_all();
}

void load_scene_async(ImporterName importer, const std::string& path, ImportSuccessFunc on_success, ImportErrorFunc on_error) {
    s_glob.job_queue.push_back({ importer, path, on_success, on_error });
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

std::shared_ptr<File> find_file(const std::string& path) {
    auto found = s_glob.text_cache.find(path);
    if (found != s_glob.text_cache.end()) {
        return found->second;
    }

    return nullptr;
}

std::shared_ptr<File> load_file_sync(const std::string& path) {
    auto found = s_glob.text_cache.find(path);
    if (found != s_glob.text_cache.end()) {
        return found->second;
    }

    auto res = FileAccess::open(path, FileAccess::READ);
    if (!res) {
        LOG_ERROR("[FileAccess] Error: failed to open file '{}', reason: {}", path, res.error().get_message());
        return nullptr;
    }

    std::shared_ptr<FileAccess> file_access = *res;

    const size_t size = file_access->get_length();

    std::vector<char> buffer;
    buffer.resize(size);
    file_access->read_buffer(buffer.data(), size);
    auto text = std::make_shared<File>();
    text->buffer = std::move(buffer);
    s_glob.text_cache[path] = text;
    return text;
}
//
// auto load_scene_assimp(const std::string& asset_path, void* data) -> std::expected<void, std::string> {
//    DEV_ASSERT(data);
//    auto scene = (reinterpret_cast<vct::Scene*>(data));
//
//    vct::SceneImporterAssimp loader(*scene, asset_path);
//    return loader.import();
//}

auto load_scene_tinygltf(const std::string& asset_path, void* data) -> std::expected<void, std::string> {
    DEV_ASSERT(data);
    auto scene = (reinterpret_cast<vct::Scene*>(data));
    vct::SceneImporterTinyGLTF loader(*scene, asset_path);
    return loader.import();
}

static void load_scene_internal(LoadTask& task) {
    LOG("[asset_loader] Loading scene '{}'...", task.asset_path);

    Scene* scene = new Scene;
    std::expected<void, std::string> res;

    Timer timer;
    if (task.importer == IMPORTER_TINYGLTF) {
        SceneImporterTinyGLTF loader(*scene, task.asset_path);
        res = loader.import();
    } else {
        SceneImporterAssimp loader(*scene, task.asset_path);
        res = loader.import();
    }

    if (!res) {
        std::string error = std::move(res.error());
        if (task.on_error) {
            task.on_error(error);
        } else {
            LOG_FATAL("{}", res.error());
        }
        return;
    }

    LOG("[asset_loader] Scene '{}' loaded in {}", task.asset_path, timer.get_duration_string());
    task.on_success(scene);
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
