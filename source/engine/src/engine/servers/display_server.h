#pragma once
#include <tuple>

#include "core/objects/singleton.h"

namespace vct {

class DisplayServer : public Singleton<DisplayServer> {
public:
    virtual bool should_close() = 0;

    virtual std::tuple<int, int> get_frame_size() = 0;
    virtual std::tuple<int, int> get_window_pos() = 0;

    virtual void new_frame() = 0;
    virtual void present() = 0;

    virtual bool initialize() = 0;
    virtual void finalize() = 0;
};

}  // namespace vct
