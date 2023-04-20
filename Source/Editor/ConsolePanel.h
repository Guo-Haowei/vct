#pragma once
#include "Engine/Framework/LogManager.h"

class ConsolePanel
{
public:
    void Render();

protected:
    bool mAutoScroll = true;
    bool mScrollToBottom = false;
    LogManager::Buffer mLogs;
};
