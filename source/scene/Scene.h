#pragma once
#include <array>
#include <memory>

#include "Camera.h"
#include "common/scene_components.h"

namespace vct {

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
    Box3 boundingBox;
    mutable bool visible = true;
};

struct GeometryNode {
    mat4 transform;
    std::vector<Geometry> geometries;
};

// TODO: light
struct Light {
    vec3 position;
    vec3 color;
};

struct Scene {
    std::vector<GeometryNode> geometryNodes;
    std::vector<std::shared_ptr<MeshComponent>> meshes;
    std::vector<std::shared_ptr<Material>> materials;
    Light light;
    Box3 boundingBox;
    Box3 shadowBox;
    Camera camera;
    bool dirty               = true;
    bool lightDirty          = true;
    const Geometry* selected = nullptr;
};

/// TODO: refactor
// B is light position
static mat4 lightSpaceMatrix( const vec3& B, const Box3& box )
{
    std::array<vec3, 8> points = {
        vec3( box.min.x, box.min.y, box.min.z ),
        vec3( box.min.x, box.min.y, box.max.z ),
        vec3( box.min.x, box.max.y, box.min.z ),
        vec3( box.min.x, box.max.y, box.max.z ),
        vec3( box.max.x, box.min.y, box.min.z ),
        vec3( box.max.x, box.min.y, box.max.z ),
        vec3( box.max.x, box.max.y, box.min.z ),
        vec3( box.max.x, box.max.y, box.max.z )
    };

    const vec3 C         = box.Center();
    float zNear          = std::numeric_limits<float>::infinity();
    float zFar           = -std::numeric_limits<float>::infinity();
    float largestTanHalf = -std::numeric_limits<float>::infinity();

    vec3 unitBC = glm::normalize( C - B );

    for ( const vec3& A : points )
    {
        vec3 BA        = A - B;
        vec3 BP        = glm::dot( unitBC, BA ) * unitBC;  // P is the projection of A on BC
        float lengthBP = glm::length( BP );
        vec3 P         = B + BP;  // P is the projection of A on BC
        zNear          = glm::min( zNear, lengthBP );
        zFar           = glm::max( zFar, lengthBP );
        largestTanHalf = glm::max( largestTanHalf, glm::length( P - A ) / lengthBP );
    }

    // const float fov = glm::atan( largestTanHalf ) * 2.0f;

    mat4 V = glm::lookAt( B, C, vec3( 1, 0, 0 ) );
    mat4 P = glm::perspectiveRH_NO( glm::radians( 90.0f ), 1.0f, zNear, zFar );
    return P * V;
}

}  // namespace vct
