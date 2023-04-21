#pragma once
#include "Panel.h"
#include "Engine/Framework/LogManager.h"

class ConsolePanel : public Panel
{
public:
    ConsolePanel() : Panel("Console") {}

protected:
    virtual void RenderInternal(Scene& scene) override;

private:
    bool mAutoScroll = true;
    bool mScrollToBottom = false;
    LogManager::Buffer mLogs;
};
