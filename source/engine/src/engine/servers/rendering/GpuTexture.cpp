#pragma once
#include "GpuTexture.h"

#include "assets/asset_loader.h"

using std::string;

void GpuTexture::bind() const { glBindTexture(m_type, mHandle); }

void GpuTexture::unbind() const { glBindTexture(m_type, 0); }

void GpuTexture::genMipMap() {
    // make sure texture is bond first
    glGenerateMipmap(m_type);
}

void GpuTexture::create2DEmpty(const Texture2DCreateInfo& info) {
    m_type = GL_TEXTURE_2D;
    m_format = info.internalFormat;

    glGenTextures(1, &mHandle);

    bind();
    glTexImage2D(m_type, 0, info.internalFormat, info.width, info.height, 0, info.format, info.dataType, 0);

    if (info.wrapS) glTexParameteri(m_type, GL_TEXTURE_WRAP_S, info.wrapS);
    if (info.wrapT) glTexParameteri(m_type, GL_TEXTURE_WRAP_T, info.wrapT);
    if (info.minFilter) glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, info.minFilter);
    if (info.magFilter) glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, info.magFilter);

    unbind();
}

void GpuTexture::create3DEmpty(const Texture3DCreateInfo& info) {
    m_type = GL_TEXTURE_3D;
    m_format = info.format;

    glGenTextures(1, &mHandle);
    bind();
    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, info.wrapS);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, info.wrapT);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_R, info.wrapR);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, info.magFilter);

    glTexStorage3D(m_type, info.mipLevel, m_format, info.size, info.size, info.size);
}

void GpuTexture::create_texture2d_from_image(const std::string& path) {
    auto image = vct::asset_loader::find_image(path);
    DEV_ASSERT(image);

    m_type = GL_TEXTURE_2D;
    glGenTextures(1, &mHandle);
    glBindTexture(m_type, mHandle);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        m_type,
        0,
        format_to_gl_internal_format(image->format),
        image->width,
        image->height,
        0,
        format_to_gl_format(image->format),
        format_to_gl_data_type(image->format),
        image->buffer.data());

    glGenerateMipmap(m_type);
    glBindTexture(m_type, 0);
}

void GpuTexture::destroy() {
    if (mHandle != 0) {
        glDeleteTextures(1, &mHandle);
    }
    mHandle = 0;
}

void GpuTexture::bindImageTexture(int i, int mipLevel) {
    glBindImageTexture(i, mHandle, mipLevel, GL_TRUE, 0, GL_READ_WRITE, m_format);
}

void GpuTexture::clear() {
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    /// Hard code format for now
    glClearTexImage(mHandle, 0, GL_RGBA, GL_FLOAT, clearColor);
    // glClearTexImage(m_handle, 0, m_format, GL_FLOAT, clearColor);
}
