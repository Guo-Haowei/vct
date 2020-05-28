#pragma once
#include "loaders/AssimpLoader.h"

class SceneManager
{
public:
    void load(const char* path);

    Scene& getScene() const { return *m_scene.get(); }
private:

    std::unique_ptr<Scene> m_scene;
};
