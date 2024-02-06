#pragma once
#include "ManagerBase.h"
#include "servers/rendering/Program.h"

enum class ProgramType {
    IMAGE2D,
    LINE3D,
    SHADOW,
    GBUFFER,
    SSAO,
    VCT_DEFERRED,
    FXAA,
    Voxel,
    Visualization,
    VoxelPost,
    DebugTexture,
    COUNT,
};

class ProgramManager : public ManagerBase {
public:
    ProgramManager() : ManagerBase("ProgramManager") {}

    const Program& GetShaderProgram(ProgramType type);

protected:
    virtual bool InitializeInternal() override;
    virtual void FinalizeInternal() override;

private:
    Program create(const ProgramCreateInfo& info);
};

extern ProgramManager* gProgramManager;