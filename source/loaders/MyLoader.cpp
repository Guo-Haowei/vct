#include "MyLoader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <streambuf>
#include <streambuf>
#include <assert.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
using namespace rapidjson;

void json2Box3D(const Value& doc, Box3D& box)
{
    box.min.x = doc["min"][0].GetFloat();
    box.min.y = doc["min"][1].GetFloat();
    box.min.z = doc["min"][2].GetFloat();
    box.max.x = doc["max"][0].GetFloat();
    box.max.y = doc["max"][1].GetFloat();
    box.max.z = doc["max"][2].GetFloat();
}

Scene* MyLoader::parse(const char* root, const char* file)
{
    using namespace std;
    // parse
    const std::string fullPath = std::string(root) + "/" + std::string(file);
    auto pos = fullPath.find(".json");
    assert(pos != string::npos);
    ifstream text(fullPath);
    if (!text.is_open())
        std::cout << "[ERROR][MyLoader] failed to open file " << fullPath << " for read." << std::endl;
    string binPath(fullPath.substr(0, pos) + ".bin");
    ifstream bin(binPath, ios::ate | ios::binary);
    if (!bin.is_open())
        std::cout << "[ERROR][MyLoader] failed to open file " << binPath << " for read." << std::endl;

    size_t sizeInByte = (size_t)bin.tellg();

    char* buffer = new char[sizeInByte];

    bin.seekg(0);
    bin.read(buffer, sizeInByte);
    bin.close();

    std::string json((std::istreambuf_iterator<char>(text)), std::istreambuf_iterator<char>());
    Document document;
    document.Parse(json.c_str());

    Scene* scene = new Scene;
    scene->name = std::string(document["name"].GetString());
    json2Box3D(document["bounding box"], scene->aabb);
    // scene->aabb
    for (auto& mesh : document["meshes"].GetArray())
    {
        std::unique_ptr<SceneMesh> sceneMesh(new SceneMesh());
        sceneMesh->name = std::string(mesh["name"].GetString());
        sceneMesh->materialIndex = mesh["material"].GetInt();
        json2Box3D(mesh["bounding box"], sceneMesh->aabb);
        auto& positions = mesh["positions"];
        assert(positions.IsObject());
        {
            size_t size = positions["size"].GetUint64();
            size_t offset = positions["offset"].GetUint64();
            assert(size % sizeof(vec3) == 0);
            const vec3* data = reinterpret_cast<const vec3*>(buffer + offset);
            sceneMesh->positions.assign(data, data + (size / sizeof(vec3)));
        }
        auto& normals = mesh["normals"];
        if (normals.IsObject())
        {
            size_t size = normals["size"].GetUint64();
            size_t offset = normals["offset"].GetUint64();
            assert(size % sizeof(vec3) == 0);
            const vec3* data = reinterpret_cast<const vec3*>(buffer + offset);
            sceneMesh->normals.assign(data, data + (size / sizeof(vec3)));
        }
        auto& indices = mesh["indices"];
        if (indices.IsObject())
        {
            size_t size = indices["size"].GetUint64();
            size_t offset = indices["offset"].GetUint64();
            assert(size % sizeof(unsigned int) == 0);
            const unsigned int* data = reinterpret_cast<const unsigned int*>(buffer + offset);
            sceneMesh->indices.assign(data, data + (size / sizeof(unsigned int)));
        }

        scene->meshes.push_back(std::move(sceneMesh));
    }
    if (document["materials"].IsArray())
    {
        for (auto& mat : document["materials"].GetArray())
        {
            std::unique_ptr<SceneMaterial> sceneMaterial(new SceneMaterial());
            sceneMaterial->name = mat["name"].GetString();
            sceneMaterial->albedoPath = mat["albedo"].GetString();
            scene->materials.push_back(std::move(sceneMaterial));
        }
    }
    auto& lightNode = document["light"];
    if (lightNode.IsObject())
    {
        auto& light = scene->light;
        auto& pos = lightNode["position"];
        //StringBuffer sb;
        //PrettyWriter<StringBuffer> writer(sb);
        //pos.Accept(writer);
        //auto str = sb.GetString();
        //std::cout << str << std::endl;
        light.position = vec3(pos[0].GetFloat(), pos[1].GetFloat(), pos[2].GetFloat());
        if (lightNode["far"].IsFloat())
            light.zFar = lightNode["far"].GetFloat();
        if (lightNode["near"].IsFloat())
            light.zNear = lightNode["near"].GetFloat();
    }

    delete [] buffer;

    return scene;
}
