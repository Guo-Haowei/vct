#pragma once
#include "core/objects/singleton.h"
#include "servers/rendering/shader_program.h"

namespace vct {

// @TODO: fix names
enum class ProgramType {
    IMAGE2D,
    SHADOW,
    GBUFFER,
    SSAO,
    VCT_DEFERRED,
    FXAA,
    Voxel,
    VoxelPost,
    DebugTexture,
    COUNT,
};

class ShaderProgramManager : public Singleton<ShaderProgramManager> {
public:
    bool initialize();
    void finalize();

    static const ShaderProgram& get(ProgramType type);

private:
    ShaderProgram create(const ProgramCreateInfo& info);
};

}  // namespace vct