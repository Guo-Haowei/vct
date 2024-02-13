#pragma once
#include "core/base/singleton.h"
#include "rendering/shader_program.h"

namespace vct {

enum ProgramType {
    PROGRAM_DPETH_STATIC,
    PROGRAM_DPETH_ANIMATED,
    PROGRAM_GBUFFER_STATIC,
    PROGRAM_GBUFFER_ANIMATED,
    PROGRAM_VOXELIZATION,
    PROGRAM_VOXELIZATION_POST,
    PROGRAM_SSAO,
    PROGRAM_LIGHTING_VXGI,
    PROGRAM_FXAA,
    PROGRAM_FINAL_IMAGE,
    PROGRAM_DEBUG_VOXEL,
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