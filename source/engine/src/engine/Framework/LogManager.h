#pragma once
#include <engine/core/collections/ring_buffer.h>

#include "Core/Singleton.h"
#include "ManagerBase.h"

class LogManager : public Singleton<LogManager>, public ManagerBase {
public:
    static constexpr int BUFFER_SIZE = 400;
    using Item = std::pair<uint32_t, std::string>;
    using Buffer = vct::RingBuffer<Item, BUFFER_SIZE>;

public:
    LogManager() : ManagerBase("LogManager") {}

    void PushLog(uint32_t color, const std::string& message);

    void GetLog(Buffer& outLogs);
    void ClearLog();

protected:
    virtual bool InitializeInternal() override;
    virtual void FinalizeInternal() override;

private:
    Buffer mLogRingBuffer;
    std::mutex mLock;
};
