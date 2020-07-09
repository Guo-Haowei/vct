#pragma once
#include "CreateScene.h"
#include "SceneLoader.h"
#include "application/Globals.h"

namespace vct {

void createDefaultScene()
{
    SceneLoader loader;
    {
        Matrix4 transform = three::scale(Vector3(0.01f));
        loader.loadGltf(DATA_DIR "models/Sponza/Sponza.gltf", g_scene, transform);
    }
    {
        Material* mat = new Material(Vector3(0.9f), 0.5f, 0.5f);
        Matrix4 transform = three::translate(Vector3(0, 1, 0)) * three::rotateY(three::radians(180.0f)) * three::scale(Vector3(2.0f));
        loader.loadObj(DATA_DIR "models/dragon.obj", g_scene, transform, mat);
    }

    Camera& camera = g_scene.camera;

    camera.fovy = three::radians(40.0f);
    camera.aspect = 1.0f;
    camera.zNear = 1.0f;
    camera.zFar = 100.0f;
    camera.yaw = three::radians(180.0f);
    camera.position = Vector3(-5, 2, 0);

    g_scene.light.position = Vector3(-20, 40, -5);

    g_UIControls.totalMeshes = static_cast<int>(g_scene.meshes.size());
    g_UIControls.totalMaterials = static_cast<int>(g_scene.materials.size());
}

Scene g_scene;

} // namespace vct
