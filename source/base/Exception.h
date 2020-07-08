#pragma once
#include <string>
#include <iostream>

namespace vct {

class Exception
{
public:
    Exception(int line, const char* file, const std::string error);
    inline const std::string& what() const { return m_message; }
protected:
    std::string m_message;
};

} // namespace vct

#ifdef _DEBUG
#   define THROW_EXCEPTION( error ) { std::cout << error << std::endl; __debugbreak(); }
#else
#   define THROW_EXCEPTION( error ) throw ::vct::Exception(__LINE__, __FILE__, error);
#endif
