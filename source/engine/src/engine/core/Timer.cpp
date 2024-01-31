#include "Timer.h"

void Timer::Start() { mStartPoint = Clock::now(); }

NanoSecond Timer::GetDuration() const {
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - mStartPoint);
    return NanoSecond(static_cast<uint64_t>(duration.count()));
}

std::string Timer::GetDurationString() const {
    auto duration = GetDuration();

    if (duration.mValue < SECOND) {
        return std::format("{:.2f} ms", duration.ToMilliSecond());
    }

    return std::format("{:.2f} seconds", duration.ToSecond());
}

ScopedTimer::ScopedTimer(const std::string& message) : mMessage(message) { Start(); }

ScopedTimer::~ScopedTimer() {
    auto duration = GetDuration();
    std::string timeString;
    if (duration.mValue < SECOND) {
        timeString = std::format("{:.2f} ms", duration.ToMilliSecond());
    } else {
        timeString = std::format("{:.2f} seconds", duration.ToSecond());
    }

    LOG("{} finished in [{}]", mMessage, timeString);
}