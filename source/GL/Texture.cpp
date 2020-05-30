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

Texture::Texture(const std::string& debugName, const CreateInfo& info)
    : GpuResource(debugName)
{
}

void Texture::internalRelease()
{
    glDeleteTextures(1, &m_handle);
}

Texture3D::Texture3D(const std::string& debugName, const Texture::CreateInfo& info)
    : Texture(debugName, info)
{
    m_type = GL_TEXTURE_3D;
    glGenTextures(1, &m_handle);
    bind();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, info.wrapS);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, info.wrapT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, info.wrapR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, info.magFilter);

    // hard code GL_RGBA8 for now
    glTexStorage3D(GL_TEXTURE_3D, info.mipLevel, GL_RGBA8, info.width, info.height, info.depth);
    unbind();
}

void Texture3D::bindToSlotForWrite(int i)
{
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_3D, m_handle);
    glBindImageTexture(i, m_handle, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
}
