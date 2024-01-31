#pragma once

#define LOG(FMT, ...)           ::vct::log_impl(::vct::LOG_LEVEL_NORMAL, FMT, ##__VA_ARGS__)
#define LOG_VERBOSE(FMT, ...)   ::vct::log_impl(::vct::LOG_LEVEL_VERBOSE, FMT, ##__VA_ARGS__)
#define LOG_WARN(FMT, ...)      ::vct::log_impl(::vct::LOG_LEVEL_WARN, FMT, ##__VA_ARGS__)
#define LOG_ERROR(FMT, ...)     ::vct::log_impl(::vct::LOG_LEVEL_ERROR, FMT, ##__VA_ARGS__)
#define LOG_FATAL(FMT, ...)     ::vct::log_impl(::vct::LOG_LEVEL_FATAL, FMT, ##__VA_ARGS__)
#define PRINT(FMT, ...)         ::vct::print_impl(::vct::LOG_LEVEL_NORMAL, FMT, ##__VA_ARGS__)
#define PRINT_VERBOSE(FMT, ...) ::vct::print_impl(::vct::LOG_LEVEL_VERBOSE, FMT, ##__VA_ARGS__)
#define PRINT_WARN(FMT, ...)    ::vct::print_impl(::vct::LOG_LEVEL_WARN, FMT, ##__VA_ARGS__)
#define PRINT_ERROR(FMT, ...)   ::vct::print_impl(::vct::LOG_LEVEL_ERROR, FMT, ##__VA_ARGS__)
#define PRINT_FATAL(FMT, ...)   ::vct::print_impl(::vct::LOG_LEVEL_FATAL, FMT, ##__VA_ARGS__)

namespace vct {

enum LogLevel : uint8_t {
    // clang-format off
    LOG_LEVEL_VERBOSE   = 0b000001,
    LOG_LEVEL_NORMAL    = 0b000010,
    LOG_LEVEL_WARN      = 0b000100,
    LOG_LEVEL_ERROR     = 0b001000,
    LOG_LEVEL_FATAL     = 0b010000,
    LOG_LEVEL_ALL       = 0b011111,
    // clang-format on
};

void print_impl(LogLevel level, const std::string& message);

template<typename... Args>
inline void print_impl(LogLevel level, std::format_string<Args...> format, Args&&... args) {
    std::string message = std::format(format, std::forward<Args>(args)...);
    print_impl(level, message);
}

void log_impl(LogLevel level, const std::string& message);

template<typename... Args>
inline void log_impl(LogLevel level, std::format_string<Args...> format, Args&&... args) {
    std::string message = std::format(format, std::forward<Args>(args)...);
    log_impl(level, message);
}

}  // namespace vct
