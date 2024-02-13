#pragma once
#include "GLPrerequisites.h"

struct Texture2DCreateInfo {
    GLenum wrapS = 0;
    GLenum wrapT = 0;
    GLenum minFilter = 0;
    GLenum magFilter = 0;
    GLenum internalFormat;
    GLenum format;
    GLenum dataType;
    int width;
    int height;
};

struct Texture3DCreateInfo {
    GLenum wrapS, wrapT, wrapR;
    GLenum minFilter, magFilter;
    GLenum format;
    int size;
    int mipLevel;
};

class GpuTexture {
public:
    void create_texture2d_from_image(const std::string& path);

    void create2DEmpty(const Texture2DCreateInfo& info);
    void create3DEmpty(const Texture3DCreateInfo& info);

    void destroy();
    void bindImageTexture(int i, int mipLevel = 0);
    void clear();
    void bind() const;
    void unbind() const;
    void genMipMap();
    inline GLenum getFormat() const { return m_format; }

    inline GLuint GetHandle() const { return mHandle; }

protected:
    GLenum m_type;
    GLenum m_format;
    GLuint mHandle = 0;

    friend class RenderTarget;
    friend class GBuffer;
};
