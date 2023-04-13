#include "Check.h"

namespace base
{

void CheckImpl(const char* file, int ln, const std::string& expr)
{
    std::string message = fmt::format(
        "*** assertion failed ***\n"
        "    {}\n"
        "    {}({},1)\n",
        expr, file, ln);

    LogInternal(ELogLevel::Fatal, message);
}

}  // namespace base
