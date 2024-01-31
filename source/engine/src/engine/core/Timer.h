#pragma once

constexpr uint64_t MILLISECOND = 1000000;
constexpr uint64_t SECOND = MILLISECOND * 1000;

struct NanoSecond {
    NanoSecond(const uint64_t value) : mValue(value) {}

    double ToMilliSecond() const {
        constexpr double factor = 1.0 / MILLISECOND;
        return factor * mValue;
    }

    double ToSecond() const {
        constexpr double factor = 1.0 / SECOND;
        return factor * mValue;
    }

    uint64_t mValue;
};

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

public:
    Timer() { Start(); }

    void Start();

    NanoSecond GetDuration() const;
    std::string GetDurationString() const;

protected:
    TimePoint mStartPoint{};
};

class ScopedTimer : public Timer {
public:
    ScopedTimer(const std::string& message);
    ~ScopedTimer();

protected:
    std::string mMessage;
};
