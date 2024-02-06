#include "thread_pool.h"

#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
    std::array<ThreadObject, THREAD_COUNT> threads = {
        ThreadObject{ "THREAD_JOB_SYSTEM_WORKER_0", jobsystem::worker_main },
        ThreadObject{ "THREAD_JOB_SYSTEM_WORKER_1", jobsystem::worker_main },
        ThreadObject{ "THREAD_JOB_SYSTEM_WORKER_2", jobsystem::worker_main },
        ThreadObject{ "THREAD_JOB_SYSTEM_WORKER_3", jobsystem::worker_main },
        ThreadObject{ "THREAD_JOB_SYSTEM_WORKER_4", jobsystem::worker_main },
        ThreadObject{ "THREAD_JOB_SYSTEM_WORKER_5", jobsystem::worker_main },
        ThreadObject{ "THREAD_JOB_SYSTEM_WORKER_6", jobsystem::worker_main },
        ThreadObject{ "THREAD_JOB_SYSTEM_WORKER_7", jobsystem::worker_main },
    };
} s_glob;

auto initialize() -> void {
    g_threadID = THREAD_MAIN;

    for (uint32_t id = 0; id < THREAD_COUNT; ++id) {
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

        LOG("[thread_pool] thread '{}'(id: {}) created.", thread.name, thread.id);
    }
}

void finailize() {
    for (auto& thread : s_glob.threads) {
        thread.thread.join();
        LOG("[thread_pool] thread '{}'(id: {}) joined.", thread.name, thread.id);
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
