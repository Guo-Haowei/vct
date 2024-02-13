#pragma once
#include "assets/scene_importer.h"
#include "core/base/singleton.h"
#include "scene/scene.h"

namespace vct {

class Application;

class ModuleBase {
public:
    ModuleBase(std::string_view name) : m_name(name) {}
    virtual ~ModuleBase() = default;

    virtual bool initialize() = 0;
    virtual void finalize() = 0;

protected:
    std::string_view m_name;
    Application* m_app;
    friend class Application;
};

class SceneManager : public Singleton<SceneManager>, public ModuleBase {
public:
    SceneManager() : ModuleBase("SceneManager") {}

    bool initialize() override;
    void finalize() override;
    void update(float dt);

    void request_scene(std::string_view path, ImporterName importer);

    void on_scene_changed(Scene* new_scene);

    void set_loading_scene(Scene* scene) {
        m_loading_scene.store(scene);
    }

    uint32_t get_revision() const { return m_revision; }

    static Scene& get_scene();

private:
    Scene* m_scene = nullptr;
    std::atomic<Scene*> m_loading_scene = nullptr;

    uint32_t m_revision = 0;
};

}  // namespace vct
