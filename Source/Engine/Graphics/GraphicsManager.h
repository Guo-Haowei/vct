#pragma once
#include "Core/ManagerBase.h"

class GraphicsManager : public ManagerBase
{
public:
    GraphicsManager() : ManagerBase("GraphicsManager") {}

protected:
    bool InitializeInternal();
    void FinalizeInternal();
};

extern GraphicsManager* gGraphicsManager;
