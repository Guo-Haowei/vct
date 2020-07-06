#pragma once
#include "ILoader.h"

struct aiMesh;
struct aiNode;
struct aiMaterial;
struct aiScene;

class AssimpLoader : public ILoader
{
public:
    virtual Scene* parse(const char* root, const char* file) override;

private:
    void processMesh(const aiMesh* mesh);

    void processMaterial(aiMaterial* material, const std::string& dir);

    // void processNode(aiNode* node);

    Scene* m_scene;
};
