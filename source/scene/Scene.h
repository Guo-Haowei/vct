#pragma once
#include "base/GeoMath.h"
#include "Camera.h"
#include <vector>
#include <memory>
#include <string>
#include <array>

namespace vct {

using PositionBuffer    = std::vector<Vector3>;
using NormalBuffer      = std::vector<Vector3>;
using UvBuffer          = std::vector<Vector2>;
using FaceBuffer        = std::vector<Vector3u>;

struct Mesh
{
    PositionBuffer  positions;
    NormalBuffer    normals;
    UvBuffer        uvs;
    FaceBuffer      faces;
    unsigned int    materialIndex;
};

struct Material
{
    /// only support albedo color for now
    std::string albedoTexture;
    std::string metallicRoughnessTexture;
    Vector3 albedo = Vector3::Zero;
    float metallic = 0.0f;
    float roughness = 0.0f;

    Material() = default;
    Material(const Vector3& albedo, float metallic, float roughness)
        : albedo(albedo), metallic(metallic), roughness(roughness)
    {}
};

struct Geometry
{
    Mesh*       pMesh;
    Material*   pMaterial;
    Box3        boundingBox;
};

struct GeometryNode
{
    Matrix4 transform;
    std::vector<Geometry> geometries;
};

// TODO: light
struct Light
{
    Vector3 position;
};

/// TODO: refactor
// B is light position
static Matrix4 lightSpaceMatrix(const Vector3& B, const Box3& box)
{
    std::array<Vector3, 8> points = {
        Vector3(box.min.x, box.min.y, box.min.z),
        Vector3(box.min.x, box.min.y, box.max.z),
        Vector3(box.min.x, box.max.y, box.min.z),
        Vector3(box.min.x, box.max.y, box.max.z),
        Vector3(box.max.x, box.min.y, box.min.z),
        Vector3(box.max.x, box.min.y, box.max.z),
        Vector3(box.max.x, box.max.y, box.min.z),
        Vector3(box.max.x, box.max.y, box.max.z)
    };

    const Vector3 C = box.getCenter();
    float zNear = std::numeric_limits<float>::infinity();
    float zFar = -std::numeric_limits<float>::infinity();
    float largestTanHalf = -std::numeric_limits<float>::infinity();

    Vector3 unitBC = three::normalize(C - B);

    for (const Vector3& A : points)
    {
        Vector3 BA = A - B;
        Vector3 BP = three::dot(unitBC, BA) * unitBC; // P is the projection of A on BC
        float lengthBP = three::length(BP);
        Vector3 P = B + BP; // P is the projection of A on BC
        zNear = std::min(zNear, lengthBP);
        zFar = std::max(zFar, lengthBP);
        largestTanHalf = std::max(largestTanHalf, three::length(P - A) / lengthBP);
    }

    Matrix4 V = three::lookAt(B, C, Vector3::UnitX);
    Matrix4 P = three::perspectiveTanHalfRH_NO(largestTanHalf, 1.0f, zNear, zFar);
    return P * V;
}

struct Scene
{
    std::vector<GeometryNode>               geometryNodes;
    std::vector<std::unique_ptr<Mesh>>      meshes;
    std::vector<std::unique_ptr<Material>>  materials;
    Light                                   light;
    Box3 boundingBox;
    Camera camera;
    bool dirty = true;
    bool lightDirty = true;
};

extern Scene g_scene;

} // namespace vct
