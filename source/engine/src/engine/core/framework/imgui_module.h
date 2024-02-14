#pragma once
#include "core/framework/module.h"

namespace vct {

class ImGuiModule : public Module {
public:
    ImGuiModule() : Module("ImGuiModule") {}

    bool initialize() override;
    void finalize() override;

protected:
    std::string m_ini_path;
};

}  // namespace vct