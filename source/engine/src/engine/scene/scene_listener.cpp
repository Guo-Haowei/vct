#pragma once
#include "scene_listener.h"
#include "SceneManager.h"

namespace vct {

void SceneListener::check_scene_update()
{
    const uint32_t revision = SceneManager::singleton().get_revision();

    if (m_cached_revision < revision) {
        LOG("[{}] detected scene change from revision: {} to revision: {}", m_debug_name, m_cached_revision, revision);
        begin_scene();
        m_cached_revision = revision;
    }
}

}  // namespace vct

