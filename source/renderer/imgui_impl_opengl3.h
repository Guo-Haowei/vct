#pragma once
#include "imgui/imgui.h"

// Backend API
IMGUI_IMPL_API bool ImGui_ImplOpenGL3_Init( const char* glsl_version = NULL );
IMGUI_IMPL_API void ImGui_ImplOpenGL3_Shutdown();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_RenderDrawData( ImDrawData* draw_data );

// (Optional) Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool ImGui_ImplOpenGL3_CreateFontsTexture();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_DestroyFontsTexture();
IMGUI_IMPL_API bool ImGui_ImplOpenGL3_CreateDeviceObjects();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_DestroyDeviceObjects();

// Specific OpenGL ES versions
//#define IMGUI_IMPL_OPENGL_ES2     // Auto-detected on Emscripten
//#define IMGUI_IMPL_OPENGL_ES3     // Auto-detected on iOS/Android

// Attempt to auto-detect the default Desktop GL loader based on available header files.
// If auto-detection fails or doesn't select the same GL loader file as used by your application,
// you are likely to get a crash in ImGui_ImplOpenGL3_Init().
// You can explicitly select a loader by using one of the '#define IMGUI_IMPL_OPENGL_LOADER_XXX' in imconfig.h or compiler command-line.
#if !defined( IMGUI_IMPL_OPENGL_ES2 ) && !defined( IMGUI_IMPL_OPENGL_ES3 ) && !defined( IMGUI_IMPL_OPENGL_LOADER_GL3W ) && !defined( IMGUI_IMPL_OPENGL_LOADER_GLEW ) && !defined( IMGUI_IMPL_OPENGL_LOADER_GLAD ) && !defined( IMGUI_IMPL_OPENGL_LOADER_GLAD2 ) && !defined( IMGUI_IMPL_OPENGL_LOADER_GLBINDING2 ) && !defined( IMGUI_IMPL_OPENGL_LOADER_GLBINDING3 ) && !defined( IMGUI_IMPL_OPENGL_LOADER_CUSTOM )

#if ( defined( __APPLE__ ) && ( TARGET_OS_IOS || TARGET_OS_TV ) ) || ( defined( __ANDROID__ ) )
#define IMGUI_IMPL_OPENGL_ES3  // iOS, Android  -> GL ES 3, "#version 300 es"
#elif defined( __EMSCRIPTEN__ )
#define IMGUI_IMPL_OPENGL_ES2  // Emscripten    -> GL ES 2, "#version 100"

// Otherwise try to detect supported Desktop OpenGL loaders..
#elif defined( __has_include )
#if __has_include( <GL/glew.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#elif __has_include( <glad/glad.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#elif __has_include( <glad/gl.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLAD2
#elif __has_include( <GL/gl3w.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GL3W
#elif __has_include( <glbinding/glbinding.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING3
#elif __has_include( <glbinding/Binding.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING2
#else
#error "Cannot detect OpenGL loader!"
#endif
#else
#define IMGUI_IMPL_OPENGL_LOADER_GL3W  // Default to GL3W embedded in our repository
#endif

#endif
