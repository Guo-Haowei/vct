#pragma once
#include "CreateScene.h"
#include "SceneLoader.h"

namespace vct {

void createDefaultScene()
{
    SceneLoader loader;
    {
        Matrix4 transform = three::scale(Vector3(0.01f));
        loader.loadObj(DATA_DIR "models/CrytekSponza/sponza.obj", g_scene, transform);
    }
    {
        Material* mat = new Material(Vector3(0.9f));
        Matrix4 transform = three::translate(Vector3(0, 1, 0)) * three::rotateY(three::radians(180.0f)) * three::scale(Vector3(2.0f));
        loader.loadObj(DATA_DIR "models/dragon.obj", g_scene, transform, mat);
    }

    Camera& camera = g_scene.camera;

    camera.fovy = three::radians(50.0f);
    camera.aspect = 1.0f;
    camera.zNear = 0.1f;
    camera.zFar = 100.0f;
    camera.yaw = three::radians(180.0f);
    camera.position = Vector3(-5, 2, 0);
}

Scene g_scene;

} // namespace vct
