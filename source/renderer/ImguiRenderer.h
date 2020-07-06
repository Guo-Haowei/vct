#pragma once
#include "GlslProgram.h"

namespace vct {

class ImguiRenderer
{
public:
    void createGpuResources();
    void render();
    void destroyGpuResources();
private:
    GlslProgram m_program;
};

} // namespace vct
