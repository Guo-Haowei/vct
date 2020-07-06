#pragma once
#include <string>
#include <vector>
#include <memory>
#include "GL/VertexArray.h"
#include "GL/Texture.h"
#include "Camera.h"

struct SceneMaterial
{
    std::string name;
    std::string albedoPath;
    std::string normalPath;
    std::unique_ptr<Texture2D> albedo;
};

struct SceneMesh
{
    std::string name;
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<vec2>  uvs;
    std::vector<unsigned int> indices;
    // std::vector<vec3>  tagents;
    // std::vector<vec3>  bitTagents;
    int materialIndex = -1;
    Box3D aabb;

    // gpu resources
    std::unique_ptr<VertexArray> vertexArray;
    std::vector<std::unique_ptr<GpuBuffer>> gpuBuffers;
};

struct Light
{
    vec3 position { 0.0f, 3000.0f, 200.0f };
    mat4 P;
    mat4 V;
    mat4 PV;
    float zNear = 0.1f;
    float zFar = 10000.0f;
};

struct Scene
{
    std::string name;
    std::vector<std::unique_ptr<SceneMesh>> meshes;
    std::vector<std::unique_ptr<SceneMaterial>> materials;
    Box3D aabb;
    PerspectiveCamera camera;
    float aabbSizeMax;
    Light light;
};