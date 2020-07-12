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
        // Material* mat = new Material(Vector3(0.9f), 0.5f, 0.03f);
        // Matrix4 transform = three::translate(Vector3(-2, 6, 0)) * three::scale(Vector3(0.03f));
        // loader.loadObj(DATA_DIR "models/sphere.obj", g_scene, transform, mat);
    }
    {
        Material* mat = new Material(Vector3(0.9f), 0.3f, 0.8f);
        Matrix4 transform = three::translate(Vector3(-0, 5, 4)) * three::rotateY(three::radians(45.0f)) * three::scale(Vector3(3));
        loader.loadObj(DATA_DIR "models/dragon.obj", g_scene, transform, mat);
    }

    Camera& camera = g_scene.camera;

    camera.fovy = three::radians(40.0f);
    camera.aspect = 1.0f;
    camera.zNear = 0.2f;
    camera.zFar = 100.0f;
    camera.yaw = three::radians(180.0f);
    camera.pitch = -0.04f;
    camera.position = Vector3(-7, 6, 4);

    g_scene.light.position = Vector3(-5.f, 25.f, -8.5f);
    g_scene.light.color = Vector3(400.0f);

    Vector3 center = g_scene.boundingBox.getCenter();
    Vector3 size = g_scene.boundingBox.getSize();

    float scale = .6f;

    size.x *= scale;
    size.z *= scale;

    g_scene.shadowBox.setFromCenterSize(center, size);

    g_UIControls.totalMeshes = static_cast<int>(g_scene.meshes.size());
    g_UIControls.totalMaterials = static_cast<int>(g_scene.materials.size());
}

Scene g_scene;

} // namespace vct
