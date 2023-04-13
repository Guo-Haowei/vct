#include "Log.h"

#include "Types.h"

#include <cstdarg>
#include <fmt/chrono.h>
#include <fmt/color.h>

#define LOG_TO_SCREEN NOT_IN_USE

#if USING(LOG_TO_SCREEN)
#include "Framework/LogManager.h"
#endif

namespace base
{

void LogInternal(ELogLevel level, const std::string& message)
{
    fmt::color color = fmt::color::silver;
    bool bold = true;
    switch (level)
    {
        case ELogLevel::Fatal:
        case ELogLevel::Error:
            color = fmt::color::red;
            break;
        case ELogLevel::Warn:
            color = fmt::color::yellow;
            break;
        case ELogLevel::Ok:
            color = fmt::color::pale_green;
            break;
        case ELogLevel::Debug:
            color = fmt::color::white;
            break;
        default:
            bold = false;
            break;
    }

#if USING(PC_PROGRAM)
    OutputDebugStringA(message.c_str());
#endif

    auto style = fg(color);
    if (bold)
    {
        style |= fmt::emphasis::bold;
    }
    fmt::print(style, "{}", message);

#if USING(LOG_TO_SCREEN)
    LogManager* pLogManager = LogManager::GetSingletonPtr();
    if (pLogManager)
    {
        pLogManager->PushLog(static_cast<uint32_t>(color), message);
    }
#endif

    if (level == ELogLevel::Fatal)
    {
        if (IsDebuggerPresent())
        {
            __debugbreak();
        }
        else
        {
            exit(1);
        }
    }
}

void Log(ELogLevel level, const std::string& message)
{
    constexpr const char* sTags[] = {
        "[FATAL]- ",
        "[ERROR]- ",
        "[WARN] - ",
        "[INFO] - ",
        "[DEBUG]- ",
        "[OK]   - ",
    };

    static_assert(array_length(sTags) == underlying(ELogLevel::Count));

    const char* tag = sTags[underlying(level)];

    std::time_t t = std::time(nullptr);
    std::string finalMessage = fmt::format("[{:%H:%M:%S}] {}{}\n", fmt::localtime(t), tag, message);

    LogInternal(level, finalMessage);
}

}  // namespace base
