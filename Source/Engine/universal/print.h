#pragma once

#include "platform_defines.h"

namespace detail {

enum class Level {
    Log = 0,
    Success,
    Info,
    Warning,
    Error,
    Fatal,
};

void Print( Level level, const char* fmt, ... );

}  // namespace detail

#define Com_Printf( fmt, ... )       ::detail::Print( ::detail::Level::Log, fmt, ##__VA_ARGS__ )
#define Com_PrintInfo( fmt, ... )    ::detail::Print( ::detail::Level::Info, fmt, ##__VA_ARGS__ )
#define Com_PrintSuccess( fmt, ... ) ::detail::Print( ::detail::Level::Success, fmt, ##__VA_ARGS__ )
#define Com_PrintWarning( fmt, ... ) ::detail::Print( ::detail::Level::Warning, fmt, ##__VA_ARGS__ )
#define Com_PrintError( fmt, ... )   ::detail::Print( ::detail::Level::Error, fmt, ##__VA_ARGS__ )
#define Com_PrintFatal( fmt, ... )   ::detail::Print( ::detail::Level::Fatal, fmt, ##__VA_ARGS__ )
