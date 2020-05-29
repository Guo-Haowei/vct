#pragma once
#include "Scene.h"
#include <fstream>

namespace JSONCpp { class JSON; }

class SceneManager
{
public:
    void load(const char* root, const char* path);
    void write();
    void createGpuResources();
    void releaseGpuResources();

    Scene& getScene() const { return *m_scene.get(); }
private:
    void writeBuffer(std::ofstream& text,
                     std::ofstream& bin,
                     const char* bufferName,
                     const void* data,
                     size_t sizeInByte,
                     size_t offset);

    std::unique_ptr<Scene> m_scene;
};

extern SceneManager* g_pSceneManager;
