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
    Texture(const std::string& debugName, const CreateInfo& info);

    virtual void internalRelease() override;

    GLenum m_type;
}; 

class Texture3D : public Texture
{
public:
    Texture3D(const std::string& debugName, const Texture::CreateInfo& info);

    void bindToSlotForWrite(int i);
};

// TODO: refactor
extern std::unique_ptr<Texture3D> g_pVoxelTexture;
