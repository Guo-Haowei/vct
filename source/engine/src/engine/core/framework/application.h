#pragma once
#include "Layer.h"
#include "ManagerBase.h"
#include "core/os/os.h"

namespace vct {

class Application {
public:
    int run(int argc, const char** argv);

    virtual void init_layers(){};

protected:
    void add_layer(std::shared_ptr<Layer> layer);

private:
    void save_command_line(int argc, const char** argv);

    std::vector<std::shared_ptr<Layer>> m_layers;
    std::vector<std::string> m_command_line;
    std::string m_app_name;

    std::shared_ptr<OS> m_os;
};

}  // namespace vct