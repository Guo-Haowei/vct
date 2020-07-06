#pragma once
#include "Exception.h"
#include <fstream>
#include <streambuf>
using std::ifstream;
using std::string;
using std::istreambuf_iterator;


namespace vct {
namespace utility {

string readAsciiFile(const char* path)
{
    ifstream file(path);
    if (!file.is_open())
        THROW_EXCEPTION("Failed to open file [" + string(path) + "]");

    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

} // namespace utility
} // namespace vct
