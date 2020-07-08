#pragma once
#include "GpuResource.h"

namespace vct {

struct Texture3DCreateInfo
{
    GLenum wrapS, wrapT, wrapR;
    GLenum minFilter, magFilter;
    GLenum format;
    int size;
    int mipLevel;
};

class DepthBuffer;

class GpuTexture : public GpuResource
{
public:
    void create3DImage(const Texture3DCreateInfo& info);
    void create2DImageFromFile(const char* path);
    void destroy();
    void bindImageTexture(int i, int mipLevel = 0);
    void clear();
    void bind() const;
    void unbind() const;
    void genMipMap();
    inline GLenum getFormat() const { return m_format; }
protected:
    GLenum m_type;
    GLenum m_format;

    friend class DepthBuffer;
};

} // namespace vct
