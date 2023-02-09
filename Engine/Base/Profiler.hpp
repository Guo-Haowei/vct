#pragma once
#include <chrono>
#include <string>

#include "Logger.h"

constexpr uint64_t MSEC = 1;
constexpr uint64_t SEC = 1000 * MSEC;

std::string FormatTime( uint64_t ms );

class Profiler {
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

public:
    void Begin();

    uint64_t GetDuration() const;

protected:
    TimePoint m_startPoint{};
};

template<typename FUNC>
class ScopeProfiler : public Profiler {
public:
    ScopeProfiler( FUNC func )
        : m_func( func )
    {
        Begin();
    }

    ~ScopeProfiler()
    {
        m_func( GetDuration() );
    }

private:
    FUNC m_func;
};

template<typename FUNC>
ScopeProfiler<FUNC> MakeScopeProfiler( FUNC f )
{
    return ScopeProfiler<FUNC>( f );
};

#define SCOPE_PROFILER( NAME, FUNC ) auto _##NAME##_profiler = MakeScopeProfiler( FUNC );
