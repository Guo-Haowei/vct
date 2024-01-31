#include "error_macros.h"

#include "core/os/os.h"

namespace vct {

static struct {
    IntrusiveList<ErrorHandlerListNode> error_handlers;
} s_glob;

void global_lock() {}
void global_unlock() {}

bool add_error_handler(ErrorHandler* handler) {
    // if the handler already exists, remove it
    remove_error_handler(handler);

    global_lock();
    s_glob.error_handlers.node_push_front(handler);
    global_unlock();
    return true;
}

bool remove_error_handler(const ErrorHandler* handler) {
    global_lock();
    s_glob.error_handlers.node_remove(handler);
    global_unlock();
    return true;
}

void report_error_impl(std::string_view function, std::string_view file, int line, std::string_view error,
                       std::string_view detail) {
    std::string extra;
    if (!detail.empty()) {
        extra = std::format("\nDetail: {}", detail);
    }

    auto message = std::format("ERROR: {}{}\n    at {} ({}:{})\n", error, extra, function, file, line);
    if (auto os = OS::singleton_ptr(); os) {
        os->print(LOG_LEVEL_ERROR, message);
    } else {
        fprintf(stderr, "%s", message.c_str());
    }

    global_lock();

    for (auto& handler : s_glob.error_handlers) {
        handler.error_func(handler.user_data, function, file, line, error);
    }

    global_unlock();
}

void report_error_index_impl(std::string_view function, std::string_view file, int line, std::string_view prefix,
                             int64_t index, int64_t bound, std::string_view index_string, std::string_view bound_string,
                             std::string_view detail) {
    auto error2 =
        std::format("{}Index {} = {} is out of bounds ({} = {}).", prefix, index_string, index, bound_string, bound);

    report_error_impl(function, file, line, error2, detail);
}

}  // namespace vct
