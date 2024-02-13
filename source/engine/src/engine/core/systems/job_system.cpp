#include "job_system.h"

#include "core/base/thread_safe_ring_buffer.h"
#include "core/math/geomath.h"
#include "core/os/threads.h"

namespace vct::jobsystem {

static struct
{
    std::condition_variable wake_condition;
    std::mutex wake_mutex;
    ThreadSafeRingBuffer<Job, 128> job_queue;
} s_glob;

static bool work() {
    Job job;
    if (!s_glob.job_queue.pop_front(job)) {
        return false;
    }

    for (uint32_t i = job.group_job_offset; i < job.group_job_end; ++i) {
        JobArgs args;
        args.group_id = job.group_id;
        args.job_index = i;
        args.group_index = i - job.group_job_offset;
        job.task(args);
    }

    job.ctx->add_task_count(-1);
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

bool initialize() {
    return true;
}

void finalize() {
    s_glob.wake_condition.notify_all();
}

void Context::dispatch(uint32_t job_count, uint32_t group_size, const std::function<void(JobArgs)>& task) {
    // DEV_ASSERT(thread::is_main_thread());

    if (job_count == 0 || group_size == 0) {
        return;
    }

    const uint32_t group_count = (job_count + group_size - 1) / group_size;  // make sure round up
    m_task_count.fetch_add(group_count);

    for (uint32_t group_id = 0; group_id < group_count; ++group_id) {
        Job job;
        job.ctx = this;
        job.task = task;
        job.group_id = group_id;
        job.group_job_offset = group_id * group_size;
        job.group_job_end = glm::min(job.group_job_offset + group_size, job_count);

        while (!s_glob.job_queue.push_back(job)) {
            // if job queue is full, notify all and let main thread do the work as well
            s_glob.wake_condition.notify_all();
            work();
        }
    }

    s_glob.wake_condition.notify_all();
}

void Context::wait() {
    // Wake any threads that might be sleeping:
    s_glob.wake_condition.notify_all();

    // Waiting will also put the current thread to good use by working on an other job if it can:
    while (is_busy()) {
        work();
    }
}

}  // namespace vct::jobsystem
