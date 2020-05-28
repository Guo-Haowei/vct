#pragma once
#include "ILoader.h"

struct aiMesh;
struct aiNode;
struct aiMaterial;

class AssimpLoader : public ILoader
{
public:
    virtual Scene* parse(const char* root, const char* file) override;

private:
    void processMesh(aiMesh* mesh);

    // void processMaterial(aiMaterial* material);

    void processNode(aiNode* node);

    Scene* m_scene;

    std::string m_dir;
};
