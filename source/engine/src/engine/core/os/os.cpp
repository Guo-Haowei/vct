#include "os.h"

#include "core/io/file_access_unix.h"

namespace vct {

void OS::initialize() {
    FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_RESOURCE);
    FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_USERDATA);
    FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);

    add_logger(std::make_shared<vct::StdLogger>());
}

void OS::finalize() {
}

void OS::add_logger(std::shared_ptr<ILogger> logger) {
    m_logger.add_logger(logger);
}

void OS::print(LogLevel level, std::string_view message) {
    m_logger.print(level, message);
    if (level == LOG_LEVEL_FATAL) {
        GENERATE_TRAP();
    }
}

}  // namespace vct