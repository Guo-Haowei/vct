#pragma once
#include "GpuResource.h"
#include "GpuTexture.h"

namespace vct {

class DepthBuffer : public GpuResource
{
public:
    void create(unsigned int width, unsigned int height);
    void bind();
    void unbind();
    void destroy();
    const GpuTexture& getDepthTexture() const { return m_depthTexture; }
private:
    GpuTexture m_depthTexture;
};

} // namespace vct
