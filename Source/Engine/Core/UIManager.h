#pragma once
#include "ManagerBase.h"

class UIManager : public ManagerBase
{
public:
    UIManager() : ManagerBase("UIManager") {}

protected:
    bool InitializeInternal();
    void FinalizeInternal();

private:
    void SetupStyle();
};

extern UIManager* gUIManager;
