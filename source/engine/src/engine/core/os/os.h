#pragma once
#include "core/base/singleton.h"
#include "core/io/logger.h"

namespace vct {

class OS : public Singleton<OS> {
public:
    void initialize();
    void finalize();

    virtual void print(LogLevel level, std::string_view message);

    void add_logger(std::shared_ptr<ILogger> logger);

protected:
    CompositeLogger m_logger;
};

}  // namespace vct