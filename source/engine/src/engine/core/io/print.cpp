#include "print.h"

#include "core/os/os.h"
#include "core/os/threads.h"

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
    const uint32_t thread_id = thread::get_thread_id();
    std::string thread_info;
    if (thread_id) {
        thread_info = std::format(" (thread id: {})", thread_id);
    }
    auto now = floor<std::chrono::seconds>(std::chrono::system_clock::now());
    std::string message_with_detail = std::format("[{:%H:%M:%S}]{} {}\n", now, thread_info, message);
    if (os) [[likely]] {
        os->print(level, message_with_detail);
    } else {
        StdLogger logger;
        logger.print(level, message_with_detail);
    }
}

}  // namespace vct
