#pragma once
#include <memory>

#include "camera.h"
#include "scene_components.h"

struct Geometry {
    std::shared_ptr<MeshComponent> mesh;
    std::shared_ptr<Material> material;
    AABB boundingBox;
    mutable bool visible = true;
};

struct GeometryNode {
    mat4 transform;
    std::vector<Geometry> geometries;
};

// TODO: light
struct Light {
    vec3 direction;
    vec3 color;
};

struct Scene {
    std::vector<GeometryNode> geometryNodes;
    std::vector<std::shared_ptr<MeshComponent>> meshes;
    std::vector<std::shared_ptr<Material>> materials;
    Light light;
    AABB boundingBox;
    Camera camera;
    bool dirty               = true;
    const Geometry* selected = nullptr;

    Scene( const Scene& ) = delete;
    Scene()               = default;
};
