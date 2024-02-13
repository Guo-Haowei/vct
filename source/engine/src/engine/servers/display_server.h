#pragma once
#include <tuple>

#include "core/base/singleton.h"
#include "core/framework/module.h"

namespace vct {

class DisplayServer : public Singleton<DisplayServer>, public Module {
public:
    DisplayServer() : Module("DisplayManager") {}

    virtual bool should_close() = 0;

    virtual std::tuple<int, int> get_frame_size() = 0;
    virtual std::tuple<int, int> get_window_pos() = 0;

    virtual void new_frame() = 0;
    virtual void present() = 0;
};

}  // namespace vct
