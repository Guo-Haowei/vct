#pragma once
#include "ManagerBase.h"

class GraphicsManager : public ManagerBase {
public:
    GraphicsManager() : ManagerBase("GraphicsManager") {}

protected:
    virtual bool InitializeInternal() override;
    virtual void FinalizeInternal() override;
};

extern GraphicsManager* gGraphicsManager;
