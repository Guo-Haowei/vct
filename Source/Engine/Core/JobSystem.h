#pragma once
#include <atomic>
#include <functional>

#include "Core/Defines.h"

namespace jobsystem
{

struct JobArgs
{
    uint32_t jobIndex;
    uint32_t groupID;
    uint32_t groupIndex;
};

class Context
{
public:
    void AddTaskCount(int num)
    {
        mTaskCount.fetch_add(num);
    }

    bool IsBusy() const
    {
        return mTaskCount.load() > 0;
    }

    void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobArgs)>& task);

    void Wait();

private:
    std::atomic_int mTaskCount = 0;
};

bool initialize();
void finalize();

}  // namespace jobsystem

#define ENABLE_JOBSYSTEM IN_USE

#if USING(ENABLE_JOBSYSTEM)
#define JS_PARALLEL_FOR(CTX, INDEX, COUNT, SUBCOUNT, BODY) \
    CTX.Dispatch(                                          \
        static_cast<uint32_t>(COUNT),                      \
        SUBCOUNT,                                          \
        [&](jobsystem::JobArgs args) { const uint32_t INDEX = args.jobIndex; do BODY while(0); })
#else
#define JS_PARALLEL_FOR(CTX, INDEX, COUNT, SUBCOUNT, BODY) \
    (void)(CTX);                                           \
    for (uint32_t INDEX = 0; INDEX < static_cast<uint32_t>(COUNT); ++INDEX) BODY
#endif
