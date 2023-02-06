#pragma once
#include "BaseDrawPass.hpp"

class GBufferPass : public BaseDrawPass {
public:
    using BaseDrawPass::BaseDrawPass;

    virtual void Draw( Frame& frame ) final;
};