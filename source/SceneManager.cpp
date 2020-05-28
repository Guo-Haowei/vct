#pragma once
#include "SceneManager.h"
#include <iostream>
#ifndef MODEL_DIR
#define MODEL_DIR ""
#endif

void SceneManager::load(const char* path)
{
    // calculate bounding box
    AssimpLoader loader;
    m_scene.reset(loader.parse(MODEL_DIR, path));

    std::cout << "[LOG][SceneManager] Calculating bounding box\n" << std::endl;
    for (auto& mesh : m_scene.get()->meshes)
    {
        for (auto& v : mesh->positions)
        {
            // mesh->aabb.expand(v);
        }

        std::cout << "\tmesh " << mesh->name << " has bbox " << mesh->aabb << std::endl;
        // m_scene->aabb.expand(mesh->aabb);
    }

    std::cout << "\tscene " << m_scene->name << " has bbox " << m_scene->aabb << std::endl;
}
