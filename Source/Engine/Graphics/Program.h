#pragma once
#include <cstdint>
#include <string>

#include "Core/ManagerBase.h"

struct ProgramCreateInfo
{
    std::string vs;
    std::string ps;
    std::string gs;
    std::string cs;
};

class Program
{
public:
    void Bind() const;
    void Unbind() const;

private:
    uint32_t mHandle = 0;

    friend class ProgramManager;
};

enum class ProgramType
{
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

class ProgramManager : public ManagerBase
{
public:
    ProgramManager() : ManagerBase("ProgramManager") {}

    const Program& GetShaderProgram(ProgramType type);

protected:
    virtual bool InitializeInternal() override;
    virtual void FinalizeInternal() override;

private:
    Program Create(const ProgramCreateInfo& info);
};

extern ProgramManager* gProgramManager;
