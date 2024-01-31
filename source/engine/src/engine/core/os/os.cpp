#include "os.h"

namespace vct {

void OS::add_logger(std::shared_ptr<ILogger> logger) {
    m_logger.add_logger(logger);
}

void OS::print(LogLevel level, std::string_view message) {
    m_logger.print(level, message);
}

}  // namespace vct