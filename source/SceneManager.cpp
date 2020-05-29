#pragma once
#include "SceneManager.h"
#include "loaders/AssimpLoader.h"
#include "loaders/MyLoader.h"
#include "internal/VectorUtils.h"

void SceneManager::load(const char* root, const char* path)
{
    // calculate bounding box
    std::string fileName(path);
    if (fileName.find(".json") != std::string::npos)
    {
        MyLoader loader;
        m_scene.reset(loader.parse(root, path));
    }
    else
    {
        AssimpLoader loader;
        m_scene.reset(loader.parse(root, path));
    }
}

void SceneManager::writeBuffer(std::ofstream& text,
                               std::ofstream& bin,
                               const char* bufferName,
                               const void* data,
                               size_t sizeInByte,
                               size_t offset)
{
    if (!data || sizeInByte == 0)
        return;
    
    text << "\"" << bufferName << "\":{\"offset\":" << offset << ", \"size\":" << sizeInByte << "},";
    bin.write((const char*)data, sizeInByte);
}

void SceneManager::write()
{
    using namespace std;
    // write txt file
    const std::string& fileName = m_scene->name;
    ofstream text(fileName + ".json");
    if (!text.is_open())
    {
        std::cout << "[ERROR] failed to open file " << fileName << ".txt for write." << std::endl;
        return;
    }

    ofstream bin(fileName + ".bin", ios::out | ios::binary);
    if (!bin.is_open())
    {
        std::cout << "[ERROR] failed to open file " << fileName << ".txt for write." << std::endl;
        return;
    }

    text << "{\"name\":\"" << fileName << "\",\"bounding box\":"<< m_scene->aabb << ",\"meshes\":[";
    unsigned int i = 0;
    size_t offset = 0;
    for (const auto& mesh : m_scene->meshes)
    {
        std::string meshName = mesh->name == "" ? "Mesh" + std::to_string(++i) : mesh->name;
        Box3D& aabb = mesh->aabb;
        text << "{\"name\":\""<< meshName << "\",\"bounding box\":" << aabb << ",";
        assert(mesh->positions.size());
        {
            size_t bufferSize = mesh->positions.size() * sizeof(vec3);
            writeBuffer(text, bin, "positions", mesh->positions.data(), bufferSize, offset);
            offset += bufferSize;
        }
        if (mesh->normals.size())
        {
            size_t bufferSize = mesh->normals.size() * sizeof(vec3);
            writeBuffer(text, bin, "normals", mesh->normals.data(), bufferSize, offset);
            offset += bufferSize;
        }
        if (mesh->indices.size())
        {
            size_t bufferSize = mesh->indices.size() * sizeof(unsigned int);
            writeBuffer(text, bin, "indices", mesh->indices.data(), bufferSize, offset);
            offset += bufferSize;
        }
        text << "},";
    }

    text << "]}\n";
    // close
    text.close();
    bin.close();
}

void SceneManager::createGpuResources()
{
    for (auto& mesh : m_scene->meshes)
    {
        const std::string& name = mesh->name;
        mesh->vertexArray.reset(new VertexArray(name, { GL_TRIANGLES }));
        mesh->vertexArray->bind();
        {
            GpuBuffer::CreateInfo vertexBufferCreateInfo {};
            vertexBufferCreateInfo.type = GL_ARRAY_BUFFER;
            vertexBufferCreateInfo.usage = GL_STATIC_DRAW;
            vertexBufferCreateInfo.initialBuffer.data = mesh->positions.data(); 
            vertexBufferCreateInfo.initialBuffer.size = sizeof(vec3) * mesh->positions.size();
            mesh->gpuBuffers.push_back(std::move(std::unique_ptr<GpuBuffer>(new GpuBuffer(name + ".position", vertexBufferCreateInfo))));
            mesh->vertexArray->appendAttribute({ GL_FLOAT, 3, sizeof(vec3), 0 }, *mesh->gpuBuffers.back().get());
        }
        if (mesh->normals.size())
        {
            GpuBuffer::CreateInfo normalBufferCreateInfo {};
            normalBufferCreateInfo.type = GL_ARRAY_BUFFER;
            normalBufferCreateInfo.usage = GL_STATIC_DRAW;
            normalBufferCreateInfo.initialBuffer.data = mesh->normals.data(); 
            normalBufferCreateInfo.initialBuffer.size = sizeof(vec3) * mesh->normals.size();
            mesh->gpuBuffers.push_back(std::move(std::unique_ptr<GpuBuffer>(new GpuBuffer(name + ".normal", normalBufferCreateInfo))));
            mesh->vertexArray->appendAttribute({ GL_FLOAT, 3, sizeof(vec3), 0 }, *mesh->gpuBuffers.back().get());
        }
        if (mesh->indices.size())
        {
            GpuBuffer::CreateInfo indexBufferCreateInfo {};
            indexBufferCreateInfo.type = GL_ELEMENT_ARRAY_BUFFER;
            indexBufferCreateInfo.usage = GL_STATIC_DRAW;
            indexBufferCreateInfo.initialBuffer.data = mesh->indices.data(); 
            indexBufferCreateInfo.initialBuffer.size = sizeof(unsigned int) * mesh->indices.size();
            mesh->gpuBuffers.push_back(std::move(std::unique_ptr<GpuBuffer>(new GpuBuffer(name + ".index", indexBufferCreateInfo))));
            mesh->vertexArray->appendIndexBuffer({ GL_UNSIGNED_INT }, *mesh->gpuBuffers.back().get());

        }
        mesh->vertexArray->unbind();
    }
}

void SceneManager::releaseGpuResources()
{
    for (auto& mesh : m_scene->meshes)
    {
        for (auto& gpuBuffer : mesh->gpuBuffers)
        {
            gpuBuffer->release();
        }
        mesh->vertexArray->release();
    }
}

SceneManager* g_pSceneManager = new SceneManager();
