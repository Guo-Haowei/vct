#include "logger.h"

#include "core/base/ring_buffer.h"

// @TODO: refactor
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace vct {

static WORD find_color_attribute(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_WARN:
            return FOREGROUND_RED | FOREGROUND_GREEN;
        case LOG_LEVEL_ERROR:
            [[fallthrough]];
        case LOG_LEVEL_FATAL:
            return FOREGROUND_RED;
        default:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}

void StdLogger::print(LogLevel level, std::string_view message) {
    const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    GetConsoleScreenBufferInfo(stdout_handle, &buffer_info);

    const WORD old_color_attrs = buffer_info.wAttributes;
    const WORD new_color = find_color_attribute(level);

    // @TODO: stderr vs stdout
    fflush(stdout);
    SetConsoleTextAttribute(stdout_handle, new_color);
    fprintf(stdout, "%.*s", static_cast<int>(message.length()), message.data());
    fflush(stdout);

    SetConsoleTextAttribute(stdout_handle, old_color_attrs);
}

void CompositeLogger::add_logger(std::shared_ptr<ILogger> logger) {
    m_loggers.emplace_back(logger);
}

void CompositeLogger::print(LogLevel level, std::string_view message) {
    // @TODO: set verbose
    if (!(m_channels & level)) {
        return;
    }

    for (auto& logger : m_loggers) {
        logger->print(level, message);
    }

    m_log_history_mutex.lock();
    m_log_history.push_back({});
    auto& log_history = m_log_history.back();
    log_history.level = level;
    strncpy(log_history.buffer, message.data(), sizeof(log_history.buffer) - 1);
    m_log_history_mutex.unlock();
}

void CompositeLogger::retrieve_log(std::vector<Log>& buffer) {
    m_log_history_mutex.lock();

    for (auto& log : m_log_history) {
        buffer.push_back(log);
    }

    m_log_history_mutex.unlock();
}

}  // namespace vct
