#pragma once
#include "BaseDrawPass.hpp"

class OverlayPass : public BaseDrawPass {
public:
    using BaseDrawPass::BaseDrawPass;

    virtual void Draw( Frame& frame ) final;
};