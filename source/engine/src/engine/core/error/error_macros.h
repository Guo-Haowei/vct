#pragma once
#include "core/base/intrusive_list.h"
#include "error.h"

namespace vct {

#ifdef _MSC_VER
#define GENERATE_TRAP() __debugbreak()
#else
#error "compiler not supported"
#endif

using ErrorHandlerFunc = void (*)(void* user_data, std::string_view function, std::string_view file, int line,
                                  std::string_view error);

struct ErrorHandlerListNode {
    ErrorHandlerFunc error_func = nullptr;
    void* user_data = nullptr;
};

using ErrorHandler = IntrusiveList<ErrorHandlerListNode>::Node;

bool add_error_handler(ErrorHandler* handler);
bool remove_error_handler(const ErrorHandler* handler);

void report_error_impl(std::string_view function, std::string_view file, int line, std::string_view error,
                       std::string_view optional_message);

inline void report_error_impl(std::string_view function, std::string_view file, int line, std::string_view error) {
    return report_error_impl(function, file, line, error, "");
}

void report_error_index_impl(std::string_view function, std::string_view file, int line, std::string_view prefix,
                             int64_t index, int64_t bound, std::string_view index_string, std::string_view bound_string,
                             std::string_view detail);

#define ERR_FAIL_V_MSG_INTERNAL(RET, MSG, EXTRA)                                                        \
    do {                                                                                                \
        vct::report_error_impl(__FUNCTION__, __FILE__, __LINE__, "Method/function failed." EXTRA, MSG); \
        return RET;                                                                                     \
    } while (0)

#define ERR_FAIL_COND_V_MSG_INTERNAL(EXPR, RET, MSG, EXTRA)                                                           \
    if (!!(EXPR)) [[unlikely]] {                                                                                      \
        vct::report_error_impl(__FUNCTION__, __FILE__, __LINE__, "Condition \"" _STR(EXPR) "\" is true." EXTRA, MSG); \
        return RET;                                                                                                   \
    } else                                                                                                            \
        ((void)0)

#define ERR_FAIL_INDEX_V_MSG_INTERNAL(INDEX, BOUND, RET, MSG)                                                        \
    if (int64_t index_ = (int64_t)(INDEX), bound_ = (int64_t)(BOUND); index_ < 0 || index_ >= bound_) [[unlikely]] { \
        vct::report_error_index_impl(__FUNCTION__, __FILE__, __LINE__, "", index_, bound_, _STR(INDEX), _STR(BOUND), \
                                     MSG);                                                                           \
        return RET;                                                                                                  \
    } else                                                                                                           \
        ((void)0)

#define ERR_FAIL()                                   ERR_FAIL_V_MSG_INTERNAL(void(), "", "")
#define ERR_FAIL_MSG(MSG)                            ERR_FAIL_V_MSG_INTERNAL(void(), MSG, "")
#define ERR_FAIL_V(RET)                              ERR_FAIL_V_MSG_INTERNAL(RET, "", " Returning: " _STR(RET))
#define ERR_FAIL_V_MSG(RET, MSG)                     ERR_FAIL_V_MSG_INTERNAL(RET, MSG, " Returning: " _STR(RET))
#define ERR_FAIL_COND(EXPR)                          ERR_FAIL_COND_V_MSG_INTERNAL(EXPR, void(), "", "")
#define ERR_FAIL_COND_MSG(EXPR, MSG)                 ERR_FAIL_COND_V_MSG_INTERNAL(EXPR, void(), MSG, "")
#define ERR_FAIL_COND_V(EXPR, RET)                   ERR_FAIL_COND_V_MSG_INTERNAL(EXPR, RET, "", " Returning: " _STR(RET))
#define ERR_FAIL_COND_V_MSG(EXPR, RET, MSG)          ERR_FAIL_COND_V_MSG_INTERNAL(EXPR, RET, MSG, " Returning: " _STR(RET))
#define ERR_FAIL_INDEX(INDEX, BOUND)                 ERR_FAIL_INDEX_V_MSG_INTERNAL(INDEX, BOUND, void(), "")
#define ERR_FAIL_INDEX_MSG(INDEX, BOUND, MSG)        ERR_FAIL_INDEX_V_MSG_INTERNAL(INDEX, BOUND, void(), MSG)
#define ERR_FAIL_INDEX_V(INDEX, BOUND, RET)          ERR_FAIL_INDEX_V_MSG_INTERNAL(INDEX, BOUND, RET, "")
#define ERR_FAIL_INDEX_V_MSG(INDEX, BOUND, RET, MSG) ERR_FAIL_INDEX_V_MSG_INTERNAL(INDEX, BOUND, RET, MSG)

// @TODO: use same crash handler
// @TODO: flush
#define CRASH_NOW()                                                                                 \
    do {                                                                                            \
        vct::report_error_impl(__FUNCTION__, __FILE__, __LINE__, "FATAL: Method/function failed."); \
        GENERATE_TRAP();                                                                            \
    } while (0)

#define CRASH_NOW_MSG(MSG)                                                                               \
    do {                                                                                                 \
        vct::report_error_impl(__FUNCTION__, __FILE__, __LINE__, "FATAL: Method/function failed.", MSG); \
        GENERATE_TRAP();                                                                                 \
    } while (0)

#define CRASH_COND(EXPR)                                                                                          \
    if (EXPR) [[unlikely]] {                                                                                      \
        vct::report_error_impl(__FUNCTION__, __FILE__, __LINE__, "FATAL: Condition \"" _STR(EXPR) "\" is true."); \
        GENERATE_TRAP();                                                                                          \
    } else                                                                                                        \
        (void)0

#define CRASH_COND_MSG(EXPR, MSG)                                                                                      \
    if (EXPR) [[unlikely]] {                                                                                           \
        vct::report_error_impl(__FUNCTION__, __FILE__, __LINE__, "FATAL: Condition \"" _STR(EXPR) "\" is true.", MSG); \
        GENERATE_TRAP();                                                                                               \
    } else                                                                                                             \
        (void)0

#define DEV_ASSERT(EXPR)                                                                                       \
    if (!(EXPR)) [[unlikely]] {                                                                                \
        vct::report_error_impl(__FUNCTION__, __FILE__, __LINE__, "FATAL: DEV_ASSERT failed (" _STR(EXPR) ")"); \
        GENERATE_TRAP();                                                                                       \
    } else                                                                                                     \
        ((void)0)

#define DEV_ASSERT_INDEX(INDEX, BOUND)                                                                               \
    if (int64_t index_ = (int64_t)(INDEX), bound_ = (int64_t)(BOUND); index_ < 0 || index_ >= bound_) [[unlikely]] { \
        vct::report_error_index_impl(__FUNCTION__, __FILE__, __LINE__, "FATAL: DEV_ASSERT_INDEX failed ", index_,    \
                                     bound_, _STR(INDEX), _STR(BOUND), "");                                          \
        GENERATE_TRAP();                                                                                             \
    } else                                                                                                           \
        ((void)0)

}  // namespace vct