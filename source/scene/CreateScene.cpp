#pragma once
#include "CreateScene.h"
#include "SceneLoader.h"

namespace vct {

void createDefaultScene()
{
    std::unique_ptr<Mesh> model(SceneLoader::loadMeshFromObj(DATA_DIR "models/dragon.obj"));
    // std::unique_ptr<Mesh> model(SceneLoader::loadMeshFromObj(DATA_DIR "models/monkey.obj"));
    g_scene.meshes.push_back(std::move(model));

    Geometry geometry;
    geometry.pMesh = g_scene.meshes.back().get();
    // geometry.boundingBox = Box3();
    GeometryNode node;
    node.transform = three::translate(Vector3(0, 0, -1)) * three::rotateY(3.14159f);
    node.geometries.push_back(geometry);

    g_scene.geometries.push_back(node);
}

Scene g_scene;

} // namespace vct
