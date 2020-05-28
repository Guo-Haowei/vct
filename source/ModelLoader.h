#pragma once
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"
#include <iostream>
#include "math/GeoMath.h"
#ifndef MODEL_DIR
#   define  MODEL_DIR ""
#endif

struct Mesh
{
    std::vector<vec3> positions;
    std::vector<unsigned int> indices;
};

static void LoadModel(const std::string& path, const std::string& name, Mesh& inMesh)
{
    std::string fullPath = MODEL_DIR + path;
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    config.triangulate = true;
    config.vertex_color = false;
    auto ret = reader.ParseFromFile(path + name, config);

    std::cout << "Loading obj [" << path << "]" << std::endl;

    if (!reader.Warning().empty())
    {
        std::cout << "Warning: " << reader.Warning() << std::endl;
    }

    if (!reader.Error().empty())
    {
        std::cout << "Error: " << reader.Error() << std::endl;
    }

    if (!ret)
    {
        return;
    }

    auto& attrib = reader.GetAttrib();
    auto& material = reader.GetMaterials();
    auto& shapes = reader.GetShapes();
    // std::cout << "Number of shapes: " << shapes.size() << std::endl;
    // std::cout << "Number of materials: " << material.size() << std::endl;
    // assume only one mesh
    auto& mesh = shapes[0].mesh;
    size_t index_offset = 0;
    for (auto& fv : mesh.num_face_vertices)
    {
        for (size_t v = 0; v < fv; ++v)
        {
            auto idx = mesh.indices[index_offset + v];
            inMesh.positions.push_back({
                attrib.vertices[3 * idx.vertex_index + 0],
                attrib.vertices[3 * idx.vertex_index + 1],
                attrib.vertices[3 * idx.vertex_index + 2]
            });
            // vertex.normal =
            // {
            //     attrib.normals[3 * idx.normal_index + 0],
            //     attrib.normals[3 * idx.normal_index + 1],
            //     attrib.normals[3 * idx.normal_index + 2]
            // };
            // if (attrib.texcoords.size())
            // {
            //     vertex.uv =
            //     {
            //         attrib.texcoords[2 * idx.texcoord_index + 0],
            //         1.0f - attrib.texcoords[2 * idx.texcoord_index + 1]
            //     };
            // }
            // vertexBuffer.push_back(vertex);
        }
        index_offset += fv;
    }
}
