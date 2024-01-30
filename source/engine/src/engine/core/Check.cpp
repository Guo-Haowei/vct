#include "Check.h"

namespace base
{

void check_impl(const char* file, int ln, const std::string& expr)
{
    std::string message = fmt::format(
        "*** assertion failed ***\n"
        "    {}\n"
        "    {}({},1)\n",
        expr, file, ln);

    log_impl(ELogLevel::Fatal, message);
}

}  // namespace base
