#include "print.h"

#include "core/os/os.h"

namespace vct {

void print_impl(LogLevel level, const std::string& message) {
    OS* os = OS::singleton_ptr();
    if (os) [[likely]] {
        os->print(level, message);
    } else {
        StdLogger logger;
        logger.print(level, message);
    }
}

void log_impl(LogLevel level, const std::string& message) {
    OS* os = OS::singleton_ptr();
    auto now = floor<std::chrono::seconds>(std::chrono::system_clock::now());
    std::string message_with_detail = std::format("[{:%H:%M:%S}] {}\n", now, message);
    if (os) [[likely]] {
        os->print(level, message_with_detail);
    } else {
        StdLogger logger;
        logger.print(level, message_with_detail);
    }
}

}  // namespace vct
