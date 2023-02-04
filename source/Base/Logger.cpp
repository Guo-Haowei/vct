#include "Logger.h"

#include <Windows.h>

#include <cstdarg>
#include <cstdio>
#include <ctime>

#define ENDPOINT stdout

namespace base {

enum {
    LOG_COLOR_WHITE = 7,
};

void Print( unsigned short style, const char* message )
{
    OutputDebugStringA( message );

    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hConsole, style );
    fprintf( ENDPOINT, message );
    SetConsoleTextAttribute( hConsole, DEFAULT_STYLE );
}

void Log( Level level, const char* fmt, ... )
{
    constexpr const char* s_tags[] = {
        "[FATAL]- ",
        "[ERROR]- ",
        "[WARN] - ",
        "[INFO] - ",
        "[DEBUG]- ",
        "[OK]   - ",
    };

    static_assert( array_length( s_tags ) == static_cast<int>( Level::Count ) );

    const char* tag = s_tags[static_cast<int>( level )];
    char buffer1[1024];
    va_list args;
    va_start( args, fmt );
    vsnprintf( buffer1, sizeof( buffer1 ), fmt, args );
    va_end( args );

    time_t rawtime;
    struct tm* timeinfo;
    std::time( &rawtime );
    timeinfo = localtime( &rawtime );
    char timebuf[128];
    strftime( timebuf, sizeof( timebuf ), "[%H:%M:%S]", timeinfo );

    char buffer2[2048];
    snprintf( buffer2, sizeof( buffer2 ), "%s %s%s\n", timebuf, tag, buffer1 );

    WORD style = DEFAULT_STYLE;
    switch ( level ) {
        case Level::Fatal:
            style = BACKGROUND_RED;
            break;
        case Level::Error:
            style = FOREGROUND_RED;
            break;
        case Level::Warn:
            style = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case Level::Ok:
            style = FOREGROUND_GREEN;
            break;
        // case Level::Info:
        //     style = FOREGROUND_BLUE;
        //     break;
        default:
            break;
    }

    Print( style, buffer2 );

    bool shouldBreak = level == Level::Fatal;
#if 0
	shouldBreak = shouldBreak || (level == Level::Error);
#endif
    if ( shouldBreak ) {
        __debugbreak();
    }
}

}  // namespace base
