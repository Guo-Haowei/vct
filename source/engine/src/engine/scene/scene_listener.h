#pragma once
#include "scene.h"

namespace vct {

class SceneListener {
protected:
    SceneListener(std::string_view debug_name)
        : m_debug_name(debug_name) {
    }

    void check_scene_update();

    virtual void begin_scene(Scene& scene) = 0;

private:
    uint32_t m_cached_revision = 0;
    std::string_view m_debug_name;
};

}  // namespace vct
