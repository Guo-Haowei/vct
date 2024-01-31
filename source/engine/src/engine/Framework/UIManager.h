#pragma once
#include "ManagerBase.h"

class UIManager : public ManagerBase {
public:
    UIManager() : ManagerBase("UIManager") {}

protected:
    virtual bool InitializeInternal() override;
    virtual void FinalizeInternal() override;

private:
    void SetupStyle();
};

extern UIManager* gUIManager;
