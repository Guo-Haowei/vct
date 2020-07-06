#pragma once
#include "SceneManager.h"
#include "loaders/AssimpLoader.h"
#include "loaders/MyLoader.h"
#include "internal/VectorUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void SceneManager::load(const char* root, const char* path)
{
    // calculate bounding box
    m_sceneRoot = root;
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

    const auto& size = m_scene->aabb.getSize();
    float aabbSizeMax = glm::max(size.x, glm::max(size.y, size.z));
    m_scene->aabbSizeMax = aabbSizeMax;

    // set light
    auto& light = m_scene->light;
    light.V = glm::lookAt(light.position, m_scene->aabb.getCenter(), vec3(0, 100, 0));
    float s = 0.5 * aabbSizeMax;
    light.P = glm::ortho(-s, s, -s, s, light.zNear, light.zFar);
    light.PV = light.P * light.V;
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
    
    text << "\"" << bufferName << "\":{\"offset\":" << offset << ", \"size\":" << sizeInByte << '}';
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
        std::string meshName = mesh->name == "" ? "Mesh" + std::to_string(i) : mesh->name;
        Box3D& aabb = mesh->aabb;
        text << "{\"name\":\""<< meshName << "\",\"bounding box\":" << aabb << ",";
        text << "\"material\":" << mesh->materialIndex << ",";
        assert(mesh->positions.size());
        {
            size_t bufferSize = mesh->positions.size() * sizeof(vec3);
            writeBuffer(text, bin, "positions", mesh->positions.data(), bufferSize, offset);
            offset += bufferSize;
            text << ',';
        }
        if (mesh->normals.size())
        {
            size_t bufferSize = mesh->normals.size() * sizeof(vec3);
            writeBuffer(text, bin, "normals", mesh->normals.data(), bufferSize, offset);
            offset += bufferSize;
            text << ',';
        }
        if (mesh->indices.size())
        {
            size_t bufferSize = mesh->indices.size() * sizeof(unsigned int);
            writeBuffer(text, bin, "indices", mesh->indices.data(), bufferSize, offset);
            offset += bufferSize;
        }
        text << "},";
        ++i;
    }

    text << "],\"materials\":[";
    i = 0;
    for (const auto& mat : m_scene->materials)
    {
        std::string matName = mat->name == "" ? "Material" + std::to_string(i) : mat->name;
        text << "{\"name\":\""<< matName << "\",";
        text << "\"index\":"<< i << ",";
        text << "\"albedo\":\"" << mat->albedoPath << "\"";
        text << ",\"normal\":\"" << mat->normalPath << "\"";
        text << "},";
        ++i;
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
        if (mesh->uvs.size())
        {
            GpuBuffer::CreateInfo uvBufferCreateInfo {};
            uvBufferCreateInfo.type = GL_ARRAY_BUFFER;
            uvBufferCreateInfo.usage = GL_STATIC_DRAW;
            uvBufferCreateInfo.initialBuffer.data = mesh->uvs.data(); 
            uvBufferCreateInfo.initialBuffer.size = sizeof(vec2) * mesh->uvs.size();
            mesh->gpuBuffers.push_back(std::move(std::unique_ptr<GpuBuffer>(new GpuBuffer(name + ".uvs", uvBufferCreateInfo))));
            mesh->vertexArray->appendAttribute({ GL_FLOAT, 2, sizeof(vec2), 0 }, *mesh->gpuBuffers.back().get());
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
    for (auto& mat : m_scene->materials)
    {
        // albedo
        const std::string& albedoPath = mat->albedoPath;
        if (albedoPath != "")
        {
            std::string path = m_sceneRoot + "/" + mat->albedoPath;
            int width, height, nrChannels;
            unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0); 
            GLenum imageFormat;
            if (nrChannels == 3) imageFormat = GL_RGB;
            else if (nrChannels == 4) imageFormat = GL_RGBA;
            else assert(0);
            if (!data)
            {
                std::cout << "[WARNNING][sbt_image] Failed to open file: " << path << std::endl;
                continue;
            }
            else
            {
                const std::string& name = mat->name;
                Texture::CreateInfo albedoCreateInfo;
                albedoCreateInfo.width = width;
                albedoCreateInfo.height = height;
                albedoCreateInfo.wrapS = albedoCreateInfo.wrapT = GL_CLAMP_TO_EDGE;
                albedoCreateInfo.minFilter = albedoCreateInfo.magFilter = GL_LINEAR;
                mat->albedo.reset(new Texture2D(name, albedoCreateInfo));
                mat->albedo->bind();
                mat->albedo->texImage2D(imageFormat, GL_RGBA, GL_UNSIGNED_BYTE, data);
                mat->albedo->generateMipMap();
                mat->albedo->unbind();
            }
        }
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
    for (auto& mat : m_scene->materials)
    {
        if (mat->albedo == nullptr)
            continue;
        mat->albedo->release();
    }
}

void SceneManager::initializeCamera()
{
    const vec3 center = m_scene->aabb.getCenter();
    const vec3 halfSize = 0.5f * m_scene->aabb.getSize();

    auto& cam = m_scene->camera;
    cam.moveUp(halfSize.y);
    float z = halfSize.y / (2.f * glm::tan(cam.getFov() / 2.f));
    m_scene->camera.moveFront(-z + center.z - halfSize.z);
    float aabbMin = glm::min(halfSize.x, glm::min(halfSize.y, halfSize.z));
    float aabbMax = glm::max(halfSize.x, glm::max(halfSize.y, halfSize.z));
    // cam.setNear(1.f);
    float zNear = 10.0f;
    float zFar = glm::max(2 * zNear, 2.0f * aabbMax);
    float speed = glm::max(aabbMin, 0.5f);
    cam.setNear(zNear);
    cam.setFar(zFar);
    cam.setSpeed(0.5f * speed);
}

SceneManager* g_pSceneManager = new SceneManager();
