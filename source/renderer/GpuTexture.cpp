#pragma once
#include "GpuTexture.h"
#include "base/Assertion.h"

namespace vct {

void GpuTexture::bind()
{
    glBindTexture(m_type, m_handle);
}

void GpuTexture::unbind()
{
    glBindTexture(m_type, 0);
}

void GpuTexture::genMipMap()
{
    // make sure texture is bond first
    glGenerateMipmap(m_type);
}

void GpuTexture::create3DImage(const Texture3DCreateInfo& info)
{
    m_type = GL_TEXTURE_3D;
    m_format = info.format;

    glGenTextures(1, &m_handle);
    glBindTexture(m_type, m_handle);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, info.wrapS);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, info.wrapT);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_R, info.wrapR);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, info.magFilter);

    glTexStorage3D(m_type, info.mipLevel, m_format, info.size, info.size, info.size);
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
    /// FIXME: are these two calls necessary?
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(m_type, m_handle);
    glBindImageTexture(i, m_handle, mipLevel, GL_TRUE, 0, GL_WRITE_ONLY, m_format);
}

void GpuTexture::clear()
{
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    /// Hard code format for now
    glClearTexImage(m_handle, 0, GL_RGBA, GL_FLOAT, clearColor);
    //glClearTexImage(m_handle, 0, m_format, GL_FLOAT, clearColor);
}

} // namespace vct

