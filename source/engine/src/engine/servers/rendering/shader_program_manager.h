#pragma once
#include "core/objects/singleton.h"
#include "servers/rendering/shader_program.h"

namespace vct {

enum ProgramType {
    PROGRAM_DPETH_STATIC,
    PROGRAM_DPETH_ANIMATED,
    PROGRAM_GBUFFER_STATIC,
    PROGRAM_GBUFFER_ANIMATED,

    // @TODO: fix names
    IMAGE2D,
    SSAO,
    VCT_DEFERRED,
    FXAA,
    Voxel,
    VoxelPost,
    DebugTexture,
    PROGRAM_MAX,
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