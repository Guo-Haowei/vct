#pragma once
#include <string>

namespace vct {
namespace utility {

extern std::string readAsciiFile(const char* path);

inline std::string readAsciiFile(const std::string& path)
{
    return readAsciiFile(path.c_str());
}

} // namespace utility
} // namespace vct
