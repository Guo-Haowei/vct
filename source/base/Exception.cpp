#include "Exception.h"
using std::string;
using std::to_string;

namespace vct {

Exception::Exception(int line, const char* file, const std::string error)
{
    m_message = "[Error] ";
    m_message
        .append(error)
        .append("\n\ton line ")
        .append(to_string(line))
        .append(" in file [")
        .append(file)
        .append("]");
}

} // namespace vct

