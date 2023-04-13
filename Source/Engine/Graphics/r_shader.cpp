#include "r_shader.h"

#include <vector>

#include "gl_utils.h"
#include "Core/Check.h"

using gl::CreateProgram;
using gl::Program;

static std::vector<Program> g_shaderCache;

void R_CreateShaderPrograms()
{
    g_shaderCache.resize(static_cast<int>(ProgramType::COUNT));

    g_shaderCache[static_cast<int>(ProgramType::LINE3D)] =
        CreateProgram(ProgramCreateInfo::VSPS("editor/line3d"));
    g_shaderCache[static_cast<int>(ProgramType::IMAGE2D)] =
        CreateProgram(ProgramCreateInfo::VSPS("editor/image"));
    g_shaderCache[static_cast<int>(ProgramType::SHADOW)] =
        CreateProgram(ProgramCreateInfo::VSPS("depth"));
    g_shaderCache[static_cast<int>(ProgramType::GBUFFER)] =
        CreateProgram(ProgramCreateInfo::VSPS("gbuffer"));
    g_shaderCache[static_cast<int>(ProgramType::SSAO)] =
        CreateProgram(ProgramCreateInfo::VSPS("fullscreen", "ssao"));
    g_shaderCache[static_cast<int>(ProgramType::VCT_DEFERRED)] =
        CreateProgram(ProgramCreateInfo::VSPS("fullscreen", "vct_deferred"));
    g_shaderCache[static_cast<int>(ProgramType::FXAA)] =
        CreateProgram(ProgramCreateInfo::VSPS("fullscreen", "fxaa"));
}

void R_DestroyShaderPrograms()
{
    for (auto& program : g_shaderCache)
    {
        program.Destroy();
    }
}

const Program& R_GetShaderProgram(ProgramType type)
{
    check(static_cast<int>(type) < g_shaderCache.size());
    return g_shaderCache[static_cast<int>(type)];
}