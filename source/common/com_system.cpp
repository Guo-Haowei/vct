#include "com_system.h"

#include <GLFW/glfw3.h>

#include "Globals.h"
#include "scene/SceneLoader.h"
#include "universal/core_assert.h"
#include "universal/print.h"

#define DEFINE_DVAR
#include "com_dvars.h"

using namespace vct;

static vct::Scene g_scene;

bool Com_RegisterDvars()
{
#define REGISTER_DVAR
#include "com_dvars.h"
    return true;
}

bool Com_LoadScene()
{
    SceneLoader loader;
    vct::Scene& scene = g_scene;

    const float worldScale = Dvar_GetFloat( scene_scale );
    const mat4 S           = glm::scale( mat4( 1 ), vec3( worldScale ) );
    const mat4 trans       = S;
    const char* scenePath  = Dvar_GetString( scene );
    loader.loadGltf( scenePath, scene, trans );

    // TODO: configure the rest
    Camera& camera = scene.camera;

    camera.fovy     = glm::radians( 60.0f );
    camera.aspect   = 1.0f;
    camera.zNear    = 0.2f;
    camera.zFar     = 100.0f;
    camera.yaw      = glm::radians( 180.0f );
    camera.pitch    = -0.04f;
    camera.position = Dvar_GetVec3( cam_pos );

    scene.light.position = vec3( -5.f, 25.f, -8.5f );
    scene.light.color    = vec3( 15.0f );

    const vec3 center     = scene.boundingBox.Center();
    vec3 size             = scene.boundingBox.Size();
    constexpr float scale = .6f;

    size.x *= scale;
    size.z *= scale;
    scene.shadowBox.FromCenterSize( center, size );

    g_UIControls.totalMeshes    = static_cast<int>( scene.meshes.size() );
    g_UIControls.totalMaterials = static_cast<int>( scene.materials.size() );

    return true;
}

vct::Scene& Com_GetScene()
{
    return g_scene;
}

//------------------------------------------------------------------------------
// Window
//------------------------------------------------------------------------------
bool Com_InitMainWindow()
{
    glfwSetErrorCallback( []( int code, const char* desc ) {
        Com_PrintFatal( "[glfw] error(%d): %s", code, desc );
    } );

    glfwInit();

    // glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    // glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    // glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    // IF_TEST( glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 ) );
    return true;
}

void Com_DestroyMainWindow()
{
    glfwTerminate();
}