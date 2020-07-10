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
        Material* mat = new Material(Vector3(0.9f), 0.2f, 0.8f);
        Matrix4 transform = three::translate(Vector3(0, 3, 0)) * three::rotateY(three::radians(180.0f)) * three::scale(Vector3(3));
        loader.loadObj(DATA_DIR "models/dragon.obj", g_scene, transform, mat);
    }

    Camera& camera = g_scene.camera;

    camera.fovy = three::radians(40.0f);
    camera.aspect = 1.0f;
    camera.zNear = 0.2f;
    camera.zFar = 100.0f;
    camera.yaw = three::radians(180.0f);
    camera.position = Vector3(-7, 2, 0);

    g_scene.light.position = Vector3(-10, 30, -5);
    g_scene.light.color = Vector3(500.0f);

    Vector3 center = g_scene.boundingBox.getCenter();
    Vector3 size = g_scene.boundingBox.getSize();

    size.x *= 0.8f;
    size.z *= 0.8f;

    g_scene.shadowBox.setFromCenterSize(center, size);

    g_UIControls.totalMeshes = static_cast<int>(g_scene.meshes.size());
    g_UIControls.totalMaterials = static_cast<int>(g_scene.materials.size());
}

Scene g_scene;

} // namespace vct
