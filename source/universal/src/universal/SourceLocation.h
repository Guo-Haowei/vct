#pragma once
#include <cstdint>
#include <string_view>

struct SourceLocation
{
    int line;
    int column; 
    std::string_view fileName;
    std::string_view functionName;
};
