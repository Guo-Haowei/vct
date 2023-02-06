#pragma once
#include <memory>

#include "Core/camera.h"
#include "SceneGraph/Entity.hpp"

// TODO: light
struct Light {
    vec3 direction;
    vec3 color;
};

class Scene {
public:
    Entity* RegisterEntity( const char* name, uint32_t flag );

    Light light;
    Camera camera;
    bool dirty = true;
    // const Geometry* selected = nullptr;

    Entity* m_root;

    AABB m_aabb;

    std::vector<std::shared_ptr<Entity>> m_entities;
    std::vector<std::shared_ptr<MeshComponent>> m_meshes;
    std::vector<std::shared_ptr<Material>> m_materials;

    Scene( const Scene& ) = delete;
    Scene() = default;
};
