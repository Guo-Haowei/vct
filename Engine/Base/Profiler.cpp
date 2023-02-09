#include "Profiler.hpp"

#include "StringUtils.hpp"

using std::string;
using namespace std::chrono;

void Profiler::Begin()
{
    m_startPoint = Clock::now();
}

uint64_t Profiler::GetDuration() const
{
    auto duration = duration_cast<milliseconds>( Clock::now() - m_startPoint );
    return static_cast<uint64_t>( duration.count() );
}

string FormatTime( uint64_t ms )
{
    char buf[128] = { 0 };
    if ( ms < 1000 ) {
        Sprintf( buf, "%llu milliseconds", ms );
    }
    else {
        Sprintf( buf, "%.2f seconds", (double)ms / SEC );
    }

    return string( buf );
}