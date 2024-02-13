#pragma once

namespace vct {

inline constexpr uint64_t kMillisecond = 1000000;
inline constexpr uint64_t kSecond = kMillisecond * 1000;

struct NanoSecond {
    NanoSecond(const uint64_t value) : m_value(value) {}

    double to_millisecond() const {
        constexpr double factor = 1.0 / kMillisecond;
        return factor * m_value;
    }

    double to_second() const {
        constexpr double factor = 1.0 / kSecond;
        return factor * m_value;
    }

    uint64_t m_value;
};

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

public:
    Timer() { start(); }

    void start();

    NanoSecond get_duration() const;
    std::string get_duration_string() const;

protected:
    TimePoint m_start_point{};
};

}  // namespace vct
