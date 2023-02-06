#pragma once
#include "BaseDrawPass.hpp"

class ShadowMapPass : public BaseDrawPass {
public:
    using BaseDrawPass::BaseDrawPass;

    virtual void Draw( Frame& frame ) final;
};