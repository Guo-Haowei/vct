#include "ProgramManager.h"

#include <sstream>
#include <vector>

#include "Core/Utility.h"
#include "Graphics/GLPrerequisites.h"

#define SHADER_FOLDER ROOT_FOLDER "/Source/Shaders"

ProgramManager *gProgramManager = new ProgramManager();

static std::string process_shader(const std::string &source) {
    std::string result;
    std::stringstream ss(source);
    for (std::string line; std::getline(ss, line);) {
        constexpr const char pattern[] = "#include";
        if (line.find(pattern) == 0) {
            const char *lineStr = line.c_str();
            const char *quote1 = strchr(lineStr, '"');
            const char *quote2 = strrchr(lineStr, '"');
            DEV_ASSERT(quote1 && quote2 && (quote1 != quote2));
            std::string includedFile(quote1 + 1, quote2);

            std::string extra = read_file_to_buffer(fs::path(SHADER_FOLDER) / includedFile);
            if (extra.empty()) {
                LOG_ERROR("[filesystem] failed to read shader '{}'", includedFile);
            }
            result.append(extra.data());
        } else {
            result.append(line);
        }

        result.push_back('\n');
    }

    return result;
}

static GLuint create_shader(const std::string &file, GLenum type) {
    std::string source = read_file_to_buffer(fs::path(SHADER_FOLDER) / file);
    if (source.empty()) {
        LOG_ERROR("[filesystem] failed to read shader '{}'", file);
        return 0;
    }

    std::string fullsource = process_shader(source);
    constexpr const char extras[] =
        "#version 460 core\n"
        "#extension GL_NV_gpu_shader5 : require\n"
        "#extension GL_NV_shader_atomic_float : enable\n"
        "#extension GL_NV_shader_atomic_fp16_vector : enable\n"
        "#extension GL_ARB_bindless_texture : require\n"
        "";
    const char *sources[] = { extras, fullsource.c_str() };

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, vct::array_length(sources), sources, nullptr);
    glCompileShader(shader);

    GLint status = GL_FALSE, length = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        std::vector<char> buffer(length + 1);
        glGetShaderInfoLog(shader, length, nullptr, buffer.data());
        LOG_ERROR("[glsl] failed to compile shader '{}'\ndetails:\n{}", file, buffer.data());
    }

    if (status == GL_FALSE) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

Program ProgramManager::Create(const ProgramCreateInfo &info) {
    GLuint programID = glCreateProgram();
    std::vector<GLuint> shaders;
    auto createShader = [&](const std::string &path, GLenum type) {
        if (!path.empty()) {
            GLuint shader = create_shader(path, type);
            glAttachShader(programID, shader);
            shaders.push_back(shader);
        }
    };

    ON_SCOPE_EXIT([&]() {
        for (GLuint id : shaders) {
            glDeleteShader(id);
        }
    });

    if (!info.cs.empty()) {
        DEV_ASSERT(info.vs.empty());
        DEV_ASSERT(info.ps.empty());
        DEV_ASSERT(info.gs.empty());
        createShader(info.cs, GL_COMPUTE_SHADER);
    } else if (!info.vs.empty()) {
        DEV_ASSERT(info.cs.empty());
        createShader(info.vs, GL_VERTEX_SHADER);
        createShader(info.ps, GL_FRAGMENT_SHADER);
        createShader(info.gs, GL_GEOMETRY_SHADER);
    }

    DEV_ASSERT(!shaders.empty());

    glLinkProgram(programID);
    GLint status = GL_FALSE, length = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        std::vector<char> buffer(length + 1);
        glGetProgramInfoLog(programID, length, nullptr, buffer.data());
        LOG_ERROR("[glsl] failed to link program\ndetails:\n{}", buffer.data());
    }

    if (status == GL_FALSE) {
        glDeleteProgram(programID);
        programID = 0;
    }

    Program program;
    program.mHandle = programID;
    return program;
}

static std::vector<Program> g_shaderCache;

bool ProgramManager::InitializeInternal() {
    g_shaderCache.resize(static_cast<int>(ProgramType::COUNT));
    {
        ProgramCreateInfo info;
        info.vs = "editor/line3d.vert";
        info.ps = "editor/line3d.frag";
        g_shaderCache[std::to_underlying(ProgramType::LINE3D)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "editor/image.vert";
        info.ps = "editor/image.frag";
        g_shaderCache[std::to_underlying(ProgramType::IMAGE2D)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "depth.vert";
        info.ps = "depth.frag";
        g_shaderCache[std::to_underlying(ProgramType::SHADOW)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "gbuffer.vert";
        info.ps = "gbuffer.frag";
        g_shaderCache[std::to_underlying(ProgramType::GBUFFER)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "fullscreen.vert";
        info.ps = "ssao.frag";
        g_shaderCache[std::to_underlying(ProgramType::SSAO)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "fullscreen.vert";
        info.ps = "vct_deferred.frag";
        g_shaderCache[std::to_underlying(ProgramType::VCT_DEFERRED)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "fullscreen.vert";
        info.ps = "fxaa.frag";
        g_shaderCache[std::to_underlying(ProgramType::FXAA)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "fullscreen.vert";
        info.ps = "debug/texture.frag";
        g_shaderCache[std::to_underlying(ProgramType::DebugTexture)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "voxel/voxelization.vert";
        info.gs = "voxel/voxelization.geom";
        info.ps = "voxel/voxelization.frag";
        g_shaderCache[std::to_underlying(ProgramType::Voxel)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.vs = "voxel/visualization.vert";
        info.ps = "voxel/visualization.frag";
        g_shaderCache[std::to_underlying(ProgramType::Visualization)] = Create(info);
    }
    {
        ProgramCreateInfo info;
        info.cs = "voxel/post.comp";
        g_shaderCache[std::to_underlying(ProgramType::VoxelPost)] = Create(info);
    }

    return true;
}

void ProgramManager::FinalizeInternal() {
    for (auto it : g_shaderCache) {
        glDeleteProgram(it.mHandle);
    }
}

const Program &ProgramManager::GetShaderProgram(ProgramType type) {
    DEV_ASSERT_INDEX(type, g_shaderCache.size());
    return g_shaderCache[std::to_underlying(type)];
}
