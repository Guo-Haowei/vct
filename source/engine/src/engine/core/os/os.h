#pragma once
#include "core/io/logger.h"
#include "core/objects/singleton.h"

namespace vct {

class OS : public Singleton<OS> {
public:
    virtual void print(LogLevel level, std::string_view message);
    void add_logger(std::shared_ptr<ILogger> logger);

protected:
    CompositeLogger m_logger;
};

}  // namespace vct