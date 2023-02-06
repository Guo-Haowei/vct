#pragma once
#include "Defines.h"

#if USING( ENABLE_LOG )
#define LOG_FATAL( fmt, ... ) base::Log( base::Level::Fatal, fmt, ##__VA_ARGS__ )
#define LOG_ERROR( fmt, ... ) base::Log( base::Level::Error, fmt, ##__VA_ARGS__ )
#define LOG_WARN( fmt, ... )  base::Log( base::Level::Warn, fmt, ##__VA_ARGS__ )
#define LOG_INFO( fmt, ... )  base::Log( base::Level::Info, fmt, ##__VA_ARGS__ )
#define LOG_DEBUG( fmt, ... ) base::Log( base::Level::Debug, fmt, ##__VA_ARGS__ )
#define LOG_OK( fmt, ... )    base::Log( base::Level::Ok, fmt, ##__VA_ARGS__ )
#else
#define LOG_FATAL( fmt, ... ) (void)0
#define LOG_ERROR( fmt, ... ) (void)0
#define LOG_WARN( fmt, ... )  (void)0
#define LOG_INFO( fmt, ... )  (void)0
#define LOG_DEBUG( fmt, ... ) (void)0
#define LOG_OK( fmt, ... )    (void)0
#endif

namespace base {

enum class Level {
    Fatal = 0,
    Error,
    Warn,
    Info,
    Debug,
    Ok,
    Count,
};

enum {
    DEFAULT_STYLE = 7,
};

void Log( Level level, const char* fmt, ... );
void Print( unsigned short style, const char* message );

}  // namespace base
