#pragma once
#include "BaseDrawPass.hpp"

class SSAOPass : public BaseDrawPass {
public:
    using BaseDrawPass::BaseDrawPass;

    virtual void Draw( Frame& frame ) final;
};