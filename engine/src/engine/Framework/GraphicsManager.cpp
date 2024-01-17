#include "GraphicsManager.h"

#include "Core/CommonDvars.h"
#include "Core/Log.h"

#include "Graphics/GLPrerequisites.h"
#include "imgui/backends/imgui_impl_opengl3.h"

GraphicsManager *gGraphicsManager = new GraphicsManager;

static void APIENTRY gl_debug_callback(GLenum, GLenum, unsigned int, GLenum, GLsizei, const char *, const void *);

bool GraphicsManager::InitializeInternal()
{
    if (gladLoadGL() == 0)
    {
        LOG_FATAL("[glad] failed to load gl functions");
        return false;
    }

    LOG_DEBUG("[opengl] renderer: {}", (const char *)glGetString(GL_RENDERER));
    LOG_DEBUG("[opengl] version: {}", (const char *)glGetString(GL_VERSION));

    if (DVAR_GET_BOOL(r_debug))
    {
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(gl_debug_callback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            LOG_DEBUG("[opengl] debug callback enabled");
        }
    }

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateDeviceObjects();
    return true;
}

void GraphicsManager::FinalizeInternal()
{
    ImGui_ImplOpenGL3_Shutdown();
}

static void APIENTRY gl_debug_callback(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char *message,
    const void *userParam)
{
    unused(length);
    unused(userParam);

    switch (id)
    {
        case 131185:
            return;
    }

    const char *sourceStr = "GL_DEBUG_SOURCE_OTHER";
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            sourceStr = "GL_DEBUG_SOURCE_API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceStr = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceStr = "GL_DEBUG_SOURCE_SHADER_COMPILER";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceStr = "GL_DEBUG_SOURCE_THIRD_PARTY";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            sourceStr = "GL_DEBUG_SOURCE_APPLICATION";
            break;
        default:
            break;
    }

    const char *typeStr = "GL_DEBUG_TYPE_OTHER";
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            typeStr = "GL_DEBUG_TYPE_ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeStr = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeStr = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeStr = "GL_DEBUG_TYPE_PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeStr = "GL_DEBUG_TYPE_PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_MARKER:
            typeStr = "GL_DEBUG_TYPE_MARKER";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            typeStr = "GL_DEBUG_TYPE_PUSH_GROUP";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            typeStr = "GL_DEBUG_TYPE_POP_GROUP";
            break;
        default:
            break;
    }

    ELogLevel level = ELogLevel::Debug;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            level = ELogLevel::Error;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            level = ELogLevel::Warn;
            break;
        default:
            break;
    }

    base::log(level, fmt::format("[opengl] {}\n\t| id: {} | source: {} | type: {}", message, id, sourceStr, typeStr));
}
