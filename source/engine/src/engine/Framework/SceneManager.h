#pragma once
#include "core/objects/singleton.h"
#include "scene/scene.h"

// @TODO: refactor
class Scene;

namespace vct {

class SceneManager : public Singleton<SceneManager> {
public:
    bool initialize();
    void finalize();
    void update(float dt);

    void request_scene(std::string_view path);

    void on_scene_changed(Scene* new_scene);

    void set_loading_scene(Scene* scene) {
        m_loading_scene.store(scene);
    }

    static Scene& get_scene();

protected:
    Scene* m_scene{ nullptr };
    std::atomic<Scene*> m_loading_scene{ nullptr };
};

}  // namespace vct

extern uint32_t g_scene_revision;
