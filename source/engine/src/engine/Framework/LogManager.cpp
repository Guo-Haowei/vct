#include "LogManager.h"

#include "Core/Check.h"

LogManager* Singleton<LogManager>::msSingleton;
static LogManager sLogManager;

bool LogManager::InitializeInternal() { return true; }

void LogManager::FinalizeInternal() { mLogRingBuffer.clear(); }

void LogManager::PushLog(uint32_t color, const std::string& message) {
    auto pair = std::make_pair(color, message);
    mLock.lock();
    mLogRingBuffer.push_back(pair);
    mLock.unlock();
}

void LogManager::GetLog(Buffer& outLogs) {
    mLock.lock();
    outLogs.clear();
    for (const auto& item : mLogRingBuffer) {
        outLogs.push_back(item);
    }
    mLock.unlock();
}

void LogManager::ClearLog() {
    mLock.lock();
    mLogRingBuffer.clear();
    mLock.unlock();
}
