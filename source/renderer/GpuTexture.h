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

class GpuTexture : public GpuResource
{
public:
    void create3DImage(const Texture3DCreateInfo& info);
    void destroy();
    void bindImageTexture(int i);
    void clear();
    void bind();
    void unbind();
    void genMipMap();
protected:
    GLenum m_type;
    GLenum m_format;
};

} // namespace vct
