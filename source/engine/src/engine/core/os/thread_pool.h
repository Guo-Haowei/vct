#pragma once

namespace vct::thread {

using ThreadMainFunc = std::function<void()>;

enum THREAD_ID : uint32_t {
    THREAD_JOB_SYSTEM_WORKER_0,
    THREAD_JOB_SYSTEM_WORKER_1,
    THREAD_JOB_SYSTEM_WORKER_2,
    THREAD_JOB_SYSTEM_WORKER_3,
    THREAD_JOB_SYSTEM_WORKER_4,
    THREAD_JOB_SYSTEM_WORKER_5,
    THREAD_JOB_SYSTEM_WORKER_6,
    THREAD_JOB_SYSTEM_WORKER_7,
    THREAD_COUNT,

    THREAD_MAIN = static_cast<uint32_t>(-1),
};

void initialize();

void finailize();

bool is_shutdown_requested();

void request_shutdown();

bool is_main_thread();

uint32_t get_thread_id();

}  // namespace vct::thread
