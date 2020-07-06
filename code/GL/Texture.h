#pragma once
#include "GpuResource.h"
#include <memory>

class Texture : public GpuResource
{
public:
    struct CreateInfo
    {
        GLenum wrapS, wrapT, wrapR;
        GLenum minFilter, magFilter;
        int width, height, depth;
        int mipLevel;
    };

    void bind();
    void unbind();
    void bindToSlot(int i);
    void generateMipMap();
    void clear(const void* color);

protected:
    Texture(const std::string& debugName, const CreateInfo& info, GLenum type);

    virtual void internalRelease() override;

    const GLenum m_type;
    int m_width, m_height;
}; 

class Texture2D : public Texture
{
public:
    Texture2D(const std::string& debugName, const Texture::CreateInfo& info);

    void texImage2D(GLenum imageFormat, GLenum textureFormat, GLenum componentFormat, const void* data = NULL);
};

class Texture3D : public Texture
{
public:
    Texture3D(const std::string& debugName, const Texture::CreateInfo& info);

    void bindToSlotForWrite(int i);

private:
    int m_depth;
};

// TODO: refactor
extern std::unique_ptr<Texture3D> g_pVoxelTexture;
extern std::unique_ptr<Texture2D> g_pShadowMap;
