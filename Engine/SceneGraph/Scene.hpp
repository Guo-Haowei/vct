#pragma once
#include <memory>
#include <unordered_map>

#include "Core/camera.h"
#include "Core/Image.hpp"
#include "SceneGraph/Entity.hpp"

// TODO: light
struct Light {
    vec3 direction;
    vec3 color;
};

class Scene {
public:
    Light light;
    Camera camera;
    bool dirty = true;
    // const Geometry* selected = nullptr;

    Entity* m_root;

    AABB m_aabb;

    std::vector<std::shared_ptr<Entity>> m_entities;
    std::vector<std::shared_ptr<MeshComponent>> m_meshes;
    std::vector<std::shared_ptr<MaterialComponent>> m_materials;
    std::unordered_map<std::string, std::shared_ptr<Image>> m_images;

    Scene( const Scene& ) = delete;
    Scene() = default;

    Entity* RegisterEntity( const char* name, uint32_t flag );

    const std::shared_ptr<Image>& GetImage( const std::string& key );

private:
};
