#pragma once
#include "BaseDrawPass.hpp"

class ForwardPass : public BaseDrawPass {
public:
    using BaseDrawPass::BaseDrawPass;

    virtual void Draw( Frame& frame ) final;
};