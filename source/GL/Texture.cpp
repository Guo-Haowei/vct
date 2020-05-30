#include "Texture.h"

void Texture::bind()
{
    glBindTexture(m_type, m_handle);
}

void Texture::unbind()
{
    glBindTexture(m_type, INVALID_HANDLE);
}

void Texture::bindToSlot(int i)
{
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(m_type, m_handle);
}

void Texture::generateMipMap()
{
    glGenerateMipmap(m_type);
}

void Texture::clear(const void* color)
{
    glClearTexImage(m_handle, 0, GL_RGBA, GL_FLOAT, color);
}

Texture::Texture(const std::string& debugName, const CreateInfo& info, GLenum type)
    : GpuResource(debugName)
    , m_width(info.width)
    , m_height(info.height)
    , m_type(type)
{
}

void Texture::internalRelease()
{
    glDeleteTextures(1, &m_handle);
}

Texture2D::Texture2D(const std::string& debugName, const Texture::CreateInfo& info)
    : Texture(debugName, info, GL_TEXTURE_2D)
{
    glGenTextures(1, &m_handle);
    bind();
    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, info.wrapS);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, info.wrapT);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, info.magFilter);
    unbind();
}

void Texture2D::texImage2D(GLenum imageFormat, GLenum textureFormat, const void* data)
{
    // hard code GL_UNSIGNED_BYTE
    glTexImage2D(m_type, 0, textureFormat, m_width, m_height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
}

Texture3D::Texture3D(const std::string& debugName, const Texture::CreateInfo& info)
    : Texture(debugName, info, GL_TEXTURE_3D), m_depth(info.depth)
{
    glGenTextures(1, &m_handle);
    bind();
    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, info.wrapS);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, info.wrapT);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_R, info.wrapR);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, info.magFilter);

    // hard code GL_RGBA8 for now
    glTexStorage3D(m_type, info.mipLevel, GL_RGBA8, info.width, info.height, info.depth);
    unbind();
}

void Texture3D::bindToSlotForWrite(int i)
{
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(m_type, m_handle);
    glBindImageTexture(i, m_handle, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
}
