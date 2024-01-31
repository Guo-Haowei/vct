#include "error.h"

namespace vct {

const char* error_to_string(ErrorCode err) {
    static const char* s_error_names[] = {
#define ERROR_CODE_NAME
#include "error_list.inl.h"
#undef ERROR_CODE_NAME
    };

    static_assert(array_length(s_error_names) == ERR_COUNT);

    return s_error_names[static_cast<int>(err)];
}

}  // namespace vct