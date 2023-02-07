#include "SceneManager.hpp"

#include <filesystem>

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Core/com_dvars.h"
#include "Core/AssimpLoader.hpp"

#ifdef max
#undef max
#endif

static Scene g_scene;

bool SceneManager::Initialize()
{
    return true;
}

bool SceneManager::LoadScene( const char* sceneName )
{
    AssimpLoader loader;
    Scene& scene = g_scene;

    const char* scenePath = sceneName;

    if ( !scenePath[0] ) {
        LOG_FATAL( "Scene not specified, set it by +set scene <name> or +exec <lua-file>" );
        return false;
    }

    scene.m_root = scene.RegisterEntity( "world", Entity::FLAG_NONE );
    scene.m_root->m_trans = glm::scale( vec3( Dvar_GetFloat( scene_scale ) ) );

    loader.loadGltf( scenePath, scene );
    scene.m_aabb.ApplyMatrix( scene.m_root->m_trans );

    Camera& camera = scene.camera;

    camera.fovy = glm::radians( Dvar_GetFloat( cam_fov ) );
    camera.zNear = 1.f;
    camera.zFar = 1000.f;

    camera.yaw = glm::radians( 180.0f );
    camera.pitch = 0.0f;
    camera.position = Dvar_GetVec3( cam_pos );

    scene.light.color = vec3( glm::clamp( Dvar_GetFloat( light_power ), 5.0f, 30.0f ) );

    LOG_OK( "Scene '%s' loaded", scenePath );
    return true;
}

Scene& Com_GetScene()
{
    return g_scene;
}
