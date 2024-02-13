#include "timer.h"

namespace vct {

void Timer::start() { m_start_point = Clock::now(); }

NanoSecond Timer::get_duration() const {
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_start_point);
    return NanoSecond(static_cast<uint64_t>(duration.count()));
}

std::string Timer::get_duration_string() const {
    auto duration = get_duration();

    if (duration.m_value < (kSecond / 10)) {
        return std::format("{:.2f} ms", duration.to_millisecond());
    }

    return std::format("{:.2f} seconds", duration.to_second());
}

}  // namespace vct