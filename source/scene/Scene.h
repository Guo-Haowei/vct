#pragma once
#include "base/GeoMath.h"
#include "Camera.h"
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
    std::vector<GeometryNode> geometryNodes;
    std::vector<std::unique_ptr<Mesh>> meshes;
    Box3 boundingBox;
    Camera camera;
    bool dirty = true;
};

extern Scene g_scene;

} // namespace vct
