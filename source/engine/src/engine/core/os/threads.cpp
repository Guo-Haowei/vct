#include "threads.h"

#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "assets/asset_loader.h"
#include "core/io/print.h"
#include "core/systems/job_system.h"

namespace vct::thread {

struct ThreadObject {
    const char* name;
    ThreadMainFunc threadMainFunc;
    uint32_t id;
    std::thread thread;
};

static thread_local uint32_t g_threadID;
static struct
{
    std::atomic_bool shutdownRequested;
    std::array<ThreadObject, THREAD_MAX> threads = {
        ThreadObject{ "main" },
        ThreadObject{ "asset loader", loader_main },
        ThreadObject{ "js worker 0", jobsystem::worker_main },
        ThreadObject{ "js worker 1", jobsystem::worker_main },
        ThreadObject{ "js worker 2", jobsystem::worker_main },
        ThreadObject{ "js worker 3", jobsystem::worker_main },
        ThreadObject{ "js worker 4", jobsystem::worker_main },
        ThreadObject{ "js worker 5", jobsystem::worker_main },
        ThreadObject{ "js worker 6", jobsystem::worker_main },
        ThreadObject{ "js worker 7", jobsystem::worker_main },
    };
} s_glob;

auto initialize() -> void {
    g_threadID = THREAD_MAIN;

    for (uint32_t id = THREAD_MAIN + 1; id < THREAD_MAX; ++id) {
        ThreadObject& thread = s_glob.threads[id];
        thread.id = id;
        thread.thread = std::thread(
            [](ThreadObject* object) {
                // set thread id
                g_threadID = object->id;
                // execute main function
                object->threadMainFunc();
            },
            &thread);

        HANDLE handle = (HANDLE)thread.thread.native_handle();

        // @TODO: set thread affinity
        // DWORD_PTR affinityMask = 1ull << threadID;
        // DWORD_PTR affinityResult = SetThreadAffinityMask(handle, affinityMask);

        std::string name = thread.name;
        std::wstring wname(name.begin(), name.end());
        HRESULT hr = SetThreadDescription(handle, wname.c_str());
        DEV_ASSERT(!FAILED(hr));

        LOG_VERBOSE("[threads] thread '{}'(id: {}) created.", thread.name, thread.id);
    }
}

void finailize() {
    for (uint32_t id = THREAD_MAIN + 1; id < THREAD_MAX; ++id) {
        auto& thread = s_glob.threads[id];
        thread.thread.join();
        LOG_VERBOSE("[threads] thread '{}'(id: {}) joined.", thread.name, thread.id);
    }
}

bool is_shutdown_requested() {
    return s_glob.shutdownRequested;
}

void request_shutdown() {
    s_glob.shutdownRequested = true;
}

bool is_main_thread() {
    return g_threadID == THREAD_MAIN;
}

uint32_t get_thread_id() {
    return g_threadID;
}

}  // namespace vct::thread
