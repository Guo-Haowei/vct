#pragma once
#include "core/base/ring_buffer.h"
// #include "core/math/color.h"
#include "core/base/singleton.h"
#include "print.h"

namespace vct {

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void print(LogLevel level, std::string_view message) = 0;
};

class StdLogger : public ILogger {
public:
    void print(LogLevel level, std::string_view message) override;
};

class CompositeLogger : public ILogger, public Singleton<CompositeLogger> {
public:
    enum {
        MAX_LOGS_KEPT = 128,
        PER_LOG_STRUCT_SIZE = 256,
    };

    struct Log {
        LogLevel level;
        char buffer[PER_LOG_STRUCT_SIZE - sizeof(level)];
    };

    void print(LogLevel level, std::string_view message) override;

    void add_logger(std::shared_ptr<ILogger> logger);
    void add_channel(LogLevel log) { m_channels |= log; }
    void remove_channel(LogLevel log) { m_channels &= ~log; }

    // @TODO: change to array
    void retrieve_log(std::vector<Log>& buffer);

private:
    std::vector<std::shared_ptr<ILogger>> m_loggers;

    RingBuffer<Log, MAX_LOGS_KEPT> m_log_history;
    std::mutex m_log_history_mutex;

    int m_channels = LOG_LEVEL_ALL;
};

}  // namespace vct
