#include "SceneManager.hpp"

#include <filesystem>

#include "Base/Asserts.h"
#include "Base/Logger.h"
#include "Base/Profiler.hpp"

#include "Core/com_dvars.h"
#include "Core/AssimpLoader.hpp"

#ifdef max
#undef max
#endif

bool SceneManager::Initialize()
{
    return true;
}

void SceneManager::Finalize()
{
}

void SceneManager::Tick()
{
}

Scene* SceneManager::GetScene() const
{
    return m_pScene.get();
};

bool SceneManager::LoadScene( const char* scene_name )
{
    bool ok = false;
    SCOPE_PROFILER( LoadScene, [&]( uint64_t ms ) {
        if ( ok ) {
            LOG_OK( "[SceneManager] Loaded scene '%s' in %s", scene_name, FormatTime( ms ).c_str() );
        }
        else {
            LOG_ERROR( "[SceneManager] Failed to load scene '%s'", scene_name );
        }
    } );

    if ( LoadAssimp( scene_name ) ) {
        ok = true;
        ++m_nSceneRevision;
    }

    return ok;
}

bool SceneManager::LoadAssimp( const char* scene_name )
{
    AssimpLoader loader;

    auto pScene = std::make_shared<Scene>();
    Scene& scene = *pScene;

    if ( !scene_name[0] ) {
        LOG_FATAL( "Scene not specified, set it by +set scene <name> or +exec <lua-file>" );
        return false;
    }

    scene.m_root = scene.RegisterEntity( "world", Entity::FLAG_NONE );
    scene.m_root->m_trans = glm::scale( vec3( Dvar_GetFloat( scene_scale ) ) );

    loader.loadGltf( scene_name, scene );
    scene.m_aabb.ApplyMatrix( scene.m_root->m_trans );

    Camera& camera = scene.camera;

    camera.fovy = glm::radians( Dvar_GetFloat( cam_fov ) );
    camera.zNear = 0.1f;
    camera.zFar = 100.f;

    camera.yaw = glm::radians( 180.0f );
    camera.pitch = 0.0f;
    camera.position = Dvar_GetVec3( cam_pos );

    scene.light.color = vec3( glm::clamp( Dvar_GetFloat( light_power ), 5.0f, 30.0f ) );

    m_pScene = std::move( pScene );
    return true;
}
