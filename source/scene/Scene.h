#pragma once
#include "base/GeoMath.h"
#include "Camera.h"
#include <vector>
#include <memory>
#include <string>

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
    std::string albedoMapPath;
    Vector3 albedoColor = Vector3::One;
    bool hasAlbedoMap = false;
    // bool hasNormalMap;

    Material() = default;
    Material(const Vector3& albedoColor)
        : albedoColor(albedoColor), hasAlbedoMap(false)
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

struct Scene
{
    std::vector<GeometryNode>               geometryNodes;
    std::vector<std::unique_ptr<Mesh>>      meshes;
    std::vector<std::unique_ptr<Material>>  materials;
    Box3 boundingBox;
    Camera camera;
    bool dirty = true;
};

extern Scene g_scene;

} // namespace vct
