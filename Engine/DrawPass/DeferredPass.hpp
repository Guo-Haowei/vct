#pragma once
#include "BaseDrawPass.hpp"

class DeferredPass : public BaseDrawPass {
public:
    using BaseDrawPass::BaseDrawPass;

    virtual void Draw( Frame& frame ) final;
};