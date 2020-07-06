#pragma once
#include "base/GeoMath.h"
#include <vector>
#include <memory>

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
};

struct Geometry
{
    Mesh* pMesh;
    // material
    Box3 boundingBox;
};

struct GeometryNode
{
    Matrix4 transform;
    std::vector<Geometry> geometries;
};

struct Scene
{
    std::vector<GeometryNode> geometries;
    std::vector<std::unique_ptr<Mesh>> meshes;
};

extern Scene g_scene;

} // namespace vct
