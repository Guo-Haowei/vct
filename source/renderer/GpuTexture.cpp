#pragma once
#include "GpuTexture.h"
#include "base/Exception.h"
#include "base/Assertion.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using std::string;

namespace vct {

void GpuTexture::bind() const
{
    glBindTexture(m_type, m_handle);
}

void GpuTexture::unbind() const
{
    glBindTexture(m_type, 0);
}

void GpuTexture::genMipMap()
{
    // make sure texture is bond first
    glGenerateMipmap(m_type);
}

void GpuTexture::create2DEmpty(const Texture2DCreateInfo& info)
{
    m_type = GL_TEXTURE_2D;
    m_format = info.internalFormat;

    glGenTextures(1, &m_handle);

    bind();
    glTexImage2D(m_type, 0, info.internalFormat, info.width, info.height, 0, info.format, info.dataType, 0);

    if (info.wrapS)
        glTexParameteri(m_type, GL_TEXTURE_WRAP_S, info.wrapS);
    if (info.wrapT)
        glTexParameteri(m_type, GL_TEXTURE_WRAP_T, info.wrapT);
    if (info.minFilter)
        glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, info.minFilter);
    if (info.magFilter)
        glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, info.magFilter);

    unbind();
}

void GpuTexture::create3DEmpty(const Texture3DCreateInfo& info)
{
    m_type = GL_TEXTURE_3D;
    m_format = info.format;

    glGenTextures(1, &m_handle);
    bind();
    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, info.wrapS);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, info.wrapT);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_R, info.wrapR);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, info.magFilter);

    glTexStorage3D(m_type, info.mipLevel, m_format, info.size, info.size, info.size);
}

void GpuTexture::create2DImageFromFile(const char* path)
{
    m_type = GL_TEXTURE_2D;
    int width, height, channel;
    unsigned char* image = stbi_load(path, &width, &height, &channel, 0);

    if (!image)
        THROW_EXCEPTION("stb: failed to load image [" + string(path) + "]");

    glGenTextures(1, &m_handle);
    glBindTexture(m_type, m_handle);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format;
    switch (channel)
    {
        case 4: format = GL_RGBA; break;
        case 3: format = GL_RGB; break;
        case 2: format = GL_RG; break;
        case 1: format = GL_RED; break;
        default: THROW_EXCEPTION("stb: unsupported image format");
    }

    glTexImage2D(m_type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(m_type);
    glBindTexture(m_type, 0);
}

void GpuTexture::destroy()
{
    if (m_handle != NULL_HANDLE)
        glDeleteTextures(1, &m_handle);
    m_handle = NULL_HANDLE;
}

void GpuTexture::bindImageTexture(int i, int mipLevel)
{
    ASSERT( m_type == GL_TEXTURE_3D );
    glBindImageTexture(i, m_handle, mipLevel, GL_TRUE, 0, GL_READ_WRITE, m_format);
}

void GpuTexture::clear()
{
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    /// Hard code format for now
    glClearTexImage(m_handle, 0, GL_RGBA, GL_FLOAT, clearColor);
    //glClearTexImage(m_handle, 0, m_format, GL_FLOAT, clearColor);
}

} // namespace vct

