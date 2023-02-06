#pragma once
#include "BaseDrawPass.hpp"

class GuiPass : public BaseDrawPass {
public:
    using BaseDrawPass::BaseDrawPass;

    virtual void BeginPass( Frame& ) override {}
    virtual void Draw( Frame& frame ) override;
    virtual void EndPass( Frame& ) override {}
};