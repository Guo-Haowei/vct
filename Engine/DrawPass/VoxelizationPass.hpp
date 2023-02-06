#pragma once
#include "BaseDrawPass.hpp"

class VoxelizationPass : public BaseDrawPass {
public:
    using BaseDrawPass::BaseDrawPass;

    virtual void Draw( Frame& frame ) override;
};