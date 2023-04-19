#pragma once
#include "Log.h"

#if USING(ENABLE_CHECK)
#define check(expr) (void)((!!(expr)) || (::base::check_impl(__FILE__, __LINE__, #expr), 0))
#define checkfmt(expr, FMT, ...)                                                             \
    do                                                                                       \
    {                                                                                        \
        if ((expr)) break;                                                                   \
        std::string message = fmt::format("{}, {}", #expr, fmt::format(FMT, ##__VA_ARGS__)); \
        ::base::check_impl(__FILE__, __LINE__, message);                                     \
    } while (0)
#define checkmsg(msg)          ::base::check_impl(__FILE__, __LINE__, msg)
#define checkrange(expr, a, b) check((((int)expr >= (int)a) && ((int)expr < (int)b)))
#define unreachable()          ::base::check_impl(__FILE__, __LINE__, "unreachable")
#else
#define check(...)       (void)0
#define checkfmt(...)    (void)0
#define checkmsg(...)    (void)0
#define checkrange(...)  (void)0
#define unreachable(...) (void)0
#endif

namespace base
{

void check_impl(const char* file, int ln, const std::string& expr);

}  // namespace base
