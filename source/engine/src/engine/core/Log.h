#pragma once
#include <fmt/format.h>

#include "Defines.h"

#if USING(ENABLE_LOG)
#define LOG_LEVEL(LEVEL, FMT, ...) ::base::log(LEVEL, ::fmt::format(FMT, ##__VA_ARGS__));

#define LOG_FATAL(FMT, ...) LOG_LEVEL(ELogLevel::Fatal, FMT, ##__VA_ARGS__);
#define LOG_ERROR(FMT, ...) LOG_LEVEL(ELogLevel::Error, FMT, ##__VA_ARGS__);
#define LOG_WARN(FMT, ...)  LOG_LEVEL(ELogLevel::Warn, FMT, ##__VA_ARGS__);
#define LOG_INFO(FMT, ...)  LOG_LEVEL(ELogLevel::Info, FMT, ##__VA_ARGS__);
#define LOG_DEBUG(FMT, ...) LOG_LEVEL(ELogLevel::Debug, FMT, ##__VA_ARGS__);
#define LOG_OK(FMT, ...)    LOG_LEVEL(ELogLevel::Ok, FMT, ##__VA_ARGS__);
#else
#define LOG_FATAL(fmt, ...) (void)0
#define LOG_ERROR(fmt, ...) (void)0
#define LOG_WARN(fmt, ...)  (void)0
#define LOG_INFO(fmt, ...)  (void)0
#define LOG_DEBUG(fmt, ...) (void)0
#define LOG_OK(fmt, ...)    (void)0
#endif

enum class ELogLevel {
    Fatal = 0,
    Error,
    Warn,
    Info,
    Debug,
    Ok,
    Count,
};

namespace base {

void log(ELogLevel level, const std::string& message);
void log_impl(ELogLevel level, const std::string& message);

}  // namespace base
