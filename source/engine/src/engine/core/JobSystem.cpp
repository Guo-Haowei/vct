#include "JobSystem.h"

#include "core/collections/fixed_stack.h"
#include "core/collections/ring_buffer.h"
#include "core/math/geomath.h"

#define WIN_CALL(x) x

namespace jobsystem {

static constexpr uint32_t MAIN_THREAD_ID = 0;
static constexpr uint32_t INVALID_WORKER_ID = MAIN_THREAD_ID;

struct Worker {
    uint32_t id = INVALID_WORKER_ID;
    std::wstring name;
    std::shared_ptr<std::thread> thread;
};

struct Job {
    Context* ctx;
    std::function<void(JobArgs)> task;
    uint32_t groupID;
    uint32_t groupJobOffset;
    uint32_t groupJobEnd;
};

// @TODO: refactor
template<size_t N>
struct ThreadSafeRingBuffer {
    vct::RingBuffer<Job, N> mRingBuffer;
    std::mutex mLock;

    bool PopFront(Job& out_value) {
        std::lock_guard<std::mutex> lock(mLock);
        if (mRingBuffer.empty()) {
            return false;
        }

        out_value = mRingBuffer.front();
        mRingBuffer.pop_front();
        return true;
    }

    bool PushBack(const Job& value) {
        std::lock_guard<std::mutex> lock(mLock);
        if (mRingBuffer.size() == mRingBuffer.capacity()) {
            return false;
        }

        mRingBuffer.push_back(value);
        return true;
    }
};

static struct {
    uint32_t numWorker;
    vct::FixedStack<Worker, 32> workers;

    std::condition_variable wakeCondition;
    std::mutex wakeMutex;
    std::atomic_bool quit;
    ThreadSafeRingBuffer<128> jobQueue;
    bool initialized;
} sJSGlob;

static thread_local uint32_t sThreadID;

static bool work();

bool initialize() {
    uint32_t numWorkers = std::thread::hardware_concurrency() - 1;
    DEV_ASSERT(numWorkers > 0);
    sJSGlob.numWorker = numWorkers;

    sThreadID = MAIN_THREAD_ID;

    for (uint32_t threadID = 1; threadID <= numWorkers; ++threadID) {
        Worker worker;
        worker.id = threadID;
        worker.name = L"JS worker" + std::to_wstring(threadID);
        worker.thread = std::make_shared<std::thread>([&] {
            sThreadID = threadID;

            while (true) {
                if (sJSGlob.quit.load()) {
                    break;
                }

                if (!work()) {
                    std::unique_lock<std::mutex> lock(sJSGlob.wakeMutex);
                    sJSGlob.wakeCondition.wait(lock);
                }
            }
        });

#if USING(PC_PROGRAM)
        HANDLE handle = (HANDLE)worker.thread->native_handle();

        DWORD_PTR affinityMask = 1ull << threadID;
        DWORD_PTR affinityResult = SetThreadAffinityMask(handle, affinityMask);
        DEV_ASSERT(affinityResult > 0);

        HRESULT hr = S_OK;
        WIN_CALL(hr = SetThreadDescription(handle, worker.name.c_str()));
#endif
        sJSGlob.workers.push_back(worker);
    }

    LOG("JobSystem has {} workers", numWorkers);
    sJSGlob.initialized = true;
    return true;
}

void finalize() {
    sJSGlob.quit.store(true);
    sJSGlob.wakeCondition.notify_all();
    for (auto& worker : sJSGlob.workers) {
        worker.thread->join();
    }
    sJSGlob.workers.clear();
}

static bool work() {
    Job job;
    if (!sJSGlob.jobQueue.PopFront(job)) {
        return false;
    }

    for (uint32_t i = job.groupJobOffset; i < job.groupJobEnd; ++i) {
        JobArgs args;
        args.groupID = job.groupID;
        args.jobIndex = i;
        args.groupIndex = i - job.groupJobOffset;
        job.task(args);
    }

    job.ctx->AddTaskCount(-1);
    return true;
}

static bool is_main_thread() { return sThreadID == MAIN_THREAD_ID; }

void Context::Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobArgs)>& task) {
    DEV_ASSERT(is_main_thread());

    if (jobCount == 0 || groupSize == 0) {
        return;
    }

    const uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;  // make sure round up
    mTaskCount.fetch_add(groupCount);

    for (uint32_t groupID = 0; groupID < groupCount; ++groupID) {
        Job job;
        job.ctx = this;
        job.task = task;
        job.groupID = groupID;
        job.groupJobOffset = groupID * groupSize;
        job.groupJobEnd = glm::min(job.groupJobOffset + groupSize, jobCount);

        while (!sJSGlob.jobQueue.PushBack(job)) {
            // if job queue is full, notify all and let main thread do the work as well
            sJSGlob.wakeCondition.notify_all();
            work();
        }
    }

    sJSGlob.wakeCondition.notify_all();
}

void Context::Wait() {
    // Wake any threads that might be sleeping:
    sJSGlob.wakeCondition.notify_all();

    // Waiting will also put the current thread to good use by working on an other job if it can:
    while (IsBusy()) {
        work();
    }
}

}  // namespace jobsystem
