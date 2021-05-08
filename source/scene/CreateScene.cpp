#pragma once
#include "CreateScene.h"

#include "SceneLoader.h"
#include "common/Globals.h"

namespace vct {

void createDefaultScene()
{
    SceneLoader loader;
    {
        mat4 transform = glm::scale( mat4( 1 ), vec3( 0.01f ) );
        loader.loadGltf( DATA_DIR "models/Sponza/Sponza.gltf", g_scene, transform );
    }

    Camera& camera = g_scene.camera;

    camera.fovy     = glm::radians( 60.0f );
    camera.aspect   = 1.0f;
    camera.zNear    = 0.2f;
    camera.zFar     = 100.0f;
    camera.yaw      = glm::radians( 180.0f );
    camera.pitch    = -0.04f;
    camera.position = vec3( -7, 6, 4 );

    g_scene.light.position = vec3( -5.f, 25.f, -8.5f );
    g_scene.light.color    = vec3( 15.0f );

    vec3 center = g_scene.boundingBox.Center();
    vec3 size   = g_scene.boundingBox.Size();

    float scale = .6f;

    size.x *= scale;
    size.z *= scale;

    g_scene.shadowBox.FromCenterSize( center, size );

    g_UIControls.totalMeshes    = static_cast<int>( g_scene.meshes.size() );
    g_UIControls.totalMaterials = static_cast<int>( g_scene.materials.size() );
}

Scene g_scene;

}  // namespace vct
