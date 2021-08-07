#pragma once
#include <memory>

#include "Camera.h"
#include "common/scene_components.h"

struct Material {
    /// only support albedo color for now
    std::string albedoTexture;
    std::string metallicRoughnessTexture;
    std::string normalTexture;
    vec3 albedo{ 0 };
    float metallic  = 0.0f;
    float roughness = 0.0f;

    Material() = default;
    Material( const vec3& albedo, float metallic, float roughness )
        : albedo( albedo ), metallic( metallic ), roughness( roughness )
    {
    }
};

struct Geometry {
    MeshComponent* pMesh;
    Material* pMaterial;
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
    AABB shadowBox;
    Camera camera;
    bool dirty               = true;
    const Geometry* selected = nullptr;
};
