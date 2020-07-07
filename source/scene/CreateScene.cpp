#pragma once
#include "CreateScene.h"
#include "SceneLoader.h"

namespace vct {

void createDefaultScene()
{
    {
        Matrix4 transform = three::scale(Vector3(0.01f));
        SceneLoader::loadObj(DATA_DIR "models/CrytekSponza/sponza.obj", g_scene, transform);
    }
    {
        Matrix4 transform = three::translate(Vector3(0, 1, 0)) * three::scale(Vector3(2.0f));
        SceneLoader::loadObj(DATA_DIR "models/dragon.obj", g_scene, transform);
    }

    Camera& camera = g_scene.camera;

    camera.fovy = three::radians(50.0f);
    camera.aspect = 1.0f;
    camera.zNear = 0.1f;
    camera.zFar = 100.0f;
    camera.position = Vector3::UnitZ;
}

Scene g_scene;

} // namespace vct
