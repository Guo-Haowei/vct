#pragma once
#include <string>

namespace vct {

class Exception
{
public:
    Exception(int line, const char* file, const std::string error);
    inline const std::string& what() { return m_message; }
protected:
    std::string m_message;
};

} // namespace vct

#define THROW_EXCEPTION( error ) throw ::vct::Exception(__LINE__, __FILE__, error);
