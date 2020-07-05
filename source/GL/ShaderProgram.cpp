#pragma once
#include "ShaderProgram.h"
#include <iostream>
#include <fstream>
#include <streambuf>

#ifndef SHADER_DIR
#define SHADER_DIR ""
#endif

ShaderProgram::ShaderProgram(const std::string& debugName, const CreateInfo& info)
    : GpuResource(debugName)
{
    auto compileShader = [](const char* path, GLenum type) -> Handle
    {
        std::ifstream file(std::string(SHADER_DIR) + std::string(path));
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + std::string(path));
        }
    
        std::string shaderSource((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        const char* shaderSourceCStr = shaderSource.c_str();

        Handle handle = glCreateShader(type);
        glShaderSource(handle, 1, &shaderSourceCStr, NULL);
        glCompileShader(handle);
        int success, logSize;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);
            char* infoLog = new char[++logSize];
            glGetShaderInfoLog(handle, logSize, NULL, infoLog);
            std::cout << "******************************************************\n";
            std::cout << "[ERROR][OpenGL] Failed to compile shader [" << path << "]\n";
            std::cout << "Error: " << infoLog << std::endl;
            delete[] infoLog;

            glDeleteShader(handle);
            return INVALID_HANDLE;
        }

        return handle;
    };

    m_handle = glCreateProgram();

    Handle hVs = compileAndAttachShader(info.vs, GL_VERTEX_SHADER);
    Handle hGs = compileAndAttachShader(info.gs, GL_GEOMETRY_SHADER);
    Handle hFs = compileAndAttachShader(info.fs, GL_FRAGMENT_SHADER);

    glLinkProgram(m_handle);
    // check linkage error
    int success, logSize;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &logSize);
        char* infoLog = new char[++logSize];
        glGetProgramInfoLog(m_handle, logSize, NULL, infoLog);
        std::cout << "******************************************************\n";
        std::cout << "[ERROR][OpenGL] Failed to link program [" << debugName << "]\n";
        std::cout << "Error: " << infoLog << std::endl;
        delete[] infoLog;
        release();
        if (hVs) glDeleteShader(hVs);
        if (hGs) glDeleteShader(hGs);
        if (hFs) glDeleteShader(hFs);
        throw std::runtime_error("");
    }

    if (hVs) glDeleteShader(hVs);
    if (hGs) glDeleteShader(hGs);
    if (hFs) glDeleteShader(hFs);
}

GpuResource::Handle ShaderProgram::compileAndAttachShader(const char* path, GLenum type)
{
    if (!path)
        return INVALID_HANDLE;

    std::ifstream file(std::string(SHADER_DIR) + std::string(path));
    if (!file.is_open())
    {
        std::cout << "******************************************************\n";
        std::cout << "[ERROR] Failed to open file [" << path << "]\n";
        return INVALID_HANDLE;
    }

    std::string shaderSource((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    const char* shaderSourceCStr = shaderSource.c_str();

    Handle handle = glCreateShader(type);
    glShaderSource(handle, 1, &shaderSourceCStr, NULL);
    glCompileShader(handle);
    int success, logSize;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);
        char* infoLog = new char[++logSize];
        glGetShaderInfoLog(handle, logSize, NULL, infoLog);
        std::cout << "******************************************************\n";
        std::cout << "[ERROR][OpenGL] Failed to compile shader [" << path << "]\n";
        std::cout << "Error: " << infoLog << std::endl;
        delete[] infoLog;

        glDeleteShader(handle);
        return INVALID_HANDLE;
    }

    glAttachShader(m_handle, handle);
    return handle;
}

void ShaderProgram::internalRelease()
{
    glDeleteProgram(m_handle);
}

void ShaderProgram::setUniform(const char* name, int val)
{
    GLint location = glGetUniformLocation(m_handle, name);
// #ifdef _DEBUG
//     if (location < 0)
//     {
//         std::cout << "[ERROR][GLSL] uniform '" << name << "' not found" << std::endl;
//     }
// #endif
    glUniform1i(location, val);
}

void ShaderProgram::setUniform(const char* name, float val)
{
    GLint location = glGetUniformLocation(m_handle, name);
// #ifdef _DEBUG
//     if (location < 0)
//     {
//         std::cout << "[ERROR][GLSL] uniform '" << name << "' not found" << std::endl;
//     }
// #endif
    glUniform1f(location, val);
}

void ShaderProgram::setUniform(const char* name, const vec3& val)
{
    GLint location = glGetUniformLocation(m_handle, name);
// #ifdef _DEBUG
//     if (location < 0)
//     {
//         std::cout << "[ERROR][GLSL] uniform '" << name << "' not found" << std::endl;
//     }
// #endif
    glUniform3f(location, val.x, val.y, val.z);
}

void ShaderProgram::setUniform(const char* name, const mat4& val)
{
    GLint location = glGetUniformLocation(m_handle, name);
// #ifdef _DEBUG
//     if (location < 0)
//     {
//         std::cout << "[ERROR][GLSL] uniform '" << name << "' not found" << std::endl;
//     }
// #endif
    glUniformMatrix4fv(location, 1, GL_FALSE, &val[0].x);
}
