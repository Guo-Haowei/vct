#include "gl_utils.h"

#include <glad/glad.h>

#include <filesystem>
#include <set>
#include <sstream>

#include "Core/CommonDvars.h"
#include "Core/Utility.h"
#include "shaders/cbuffer.glsl"
#include "Core/Check.h"
#include "Core/DynamicVariable.h"
#include "Core/Log.h"

#define SHADER_FOLDER ROOT_FOLDER "/Source/Shaders"

static MeshData g_quad;

void R_CreateQuad()
{
    // clang-format off
    float points[] = { -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, };
    // clang-format on
    glGenVertexArrays(1, &g_quad.vao);
    glGenBuffers(1, g_quad.vbos);
    glBindVertexArray(g_quad.vao);

    glBindBuffer(GL_ARRAY_BUFFER, g_quad.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
}

void R_DrawQuad()
{
    check(g_quad.vao);
    glBindVertexArray(g_quad.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FillMaterialCB(const MaterialData *mat, MaterialCB &cb)
{
    cb.AlbedoColor = mat->albedoColor;
    cb.Metallic = mat->metallic;
    cb.Roughness = mat->roughness;
    cb.HasAlbedoMap = mat->albedoMap.GetHandle() != 0;
    cb.HasNormalMap = mat->materialMap.GetHandle() != 0;
    cb.HasPbrMap = mat->materialMap.GetHandle() != 0;
    cb.TextureMapIdx = mat->textureMapIdx;
    cb.ReflectPower = mat->reflectPower;
}

namespace gl
{

//------------------------------------------------------------------------------
// Shader
//------------------------------------------------------------------------------

static std::string process_shader(const std::string &source)
{
    std::string result;
    std::stringstream ss(source);
    for (std::string line; std::getline(ss, line);)
    {
        constexpr const char pattern[] = "#include";
        if (line.find(pattern) == 0)
        {
            const char *lineStr = line.c_str();
            const char *quote1 = strchr(lineStr, '"');
            const char *quote2 = strrchr(lineStr, '"');
            check(quote1 && quote2 && (quote1 != quote2));
            std::string includedFile(quote1 + 1, quote2);

            std::string extra = read_file_to_buffer(fs::path(SHADER_FOLDER) / includedFile);
            if (extra.empty())
            {
                LOG_ERROR("[filesystem] failed to read shader '{}'", includedFile);
            }
            result.append(extra.data());
        }
        else
        {
            result.append(line);
        }

        result.push_back('\n');
    }

    return result;
}

static GLuint create_shader(const char *file, GLenum type)
{
    std::string source = read_file_to_buffer(fs::path(SHADER_FOLDER) / file);
    if (source.empty())
    {
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
    glShaderSource(shader, array_length(sources), sources, nullptr);
    glCompileShader(shader);

    GLint status = GL_FALSE, length = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0)
    {
        std::vector<char> buffer(length + 1);
        glGetShaderInfoLog(shader, length, nullptr, buffer.data());
        LOG_ERROR("[glsl] failed to compile shader '{}'\ndetails:\n{}", file, buffer.data());
    }

    if (status == GL_FALSE)
    {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint CreateProgram(const ProgramCreateInfo &info)
{
    GLuint program = 0;
    const char *name = "<unknown>";

    GLuint vs, ps, gs, cs;

    if (!info.cs.empty())
    {
        check(info.vs.empty());
        check(info.ps.empty());
        check(info.gs.empty());

        LOG_INFO("compiling compute ({}) pipeline", info.cs.c_str());
        program = glCreateProgram();
        name = info.cs.c_str();
        cs = create_shader(info.cs.c_str(), GL_COMPUTE_SHADER);
        glAttachShader(program, cs);
    }
    else if (info.vs[0] && info.ps[0])
    {
        LOG_INFO("compiling vertex ({}), geometry({}), pixel({})", info.vs.c_str(), info.gs.c_str(), info.ps.c_str());

        program = glCreateProgram();
        name = info.vs.c_str();

        vs = create_shader(info.vs.c_str(), GL_VERTEX_SHADER);
        glAttachShader(program, vs);
        ps = create_shader(info.ps.c_str(), GL_FRAGMENT_SHADER);
        glAttachShader(program, ps);
        if (!info.gs.empty())
        {
            gs = create_shader(info.gs.c_str(), GL_GEOMETRY_SHADER);
            glAttachShader(program, gs);
        }
    }

    check(program);

    glLinkProgram(program);
    GLint status = GL_FALSE, length = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    if (length > 0)
    {
        std::vector<char> buffer(length + 1);
        glGetProgramInfoLog(program, length, nullptr, buffer.data());
        LOG_ERROR("[glsl] failed to link program '{}'\ndetails:\n{}", name, buffer.data());
    }

    if (status == GL_FALSE)
    {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

//------------------------------------------------------------------------------
// Constant Buffer
//------------------------------------------------------------------------------
GLuint CreateAndBindConstantBuffer(int slot, size_t sizeInByte)
{
    GLuint handle = 0;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeInByte, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, handle);
    LOG_DEBUG("[opengl] created buffer of size {} (slot {})", sizeInByte, slot);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return handle;
}

void UpdateConstantBuffer(GLuint handle, const void *ptr, size_t sizeInByte)
{
    // glMapBuffer( mHandle, 0 );
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeInByte, ptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

}  // namespace gl