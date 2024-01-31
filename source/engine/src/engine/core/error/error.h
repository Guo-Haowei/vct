#pragma once

namespace vct {

enum ErrorCode : uint16_t {
#define ERROR_CODE_ENUM
#include "error_list.inl.h"
#undef ERROR_CODE_ENUM
    ERR_COUNT,
};

struct ErrorBase {
    ErrorBase(std::string_view filepath, std::string_view function, int line)
        : filepath(filepath), function(function), line(line) {}

    std::string_view filepath;
    std::string_view function;
    int line;
};

template<typename T>
class Error : public ErrorBase {
public:
    Error(std::string_view filepath, std::string_view function, int line, const T& value)
        : ErrorBase(filepath, function, line), m_value(value) {}

    template<typename... Args>
    Error(std::string_view filepath, std::string_view function, int line, const T& value,
          std::format_string<Args...> format, Args&&... args)
        : Error(filepath, function, line, value) {
        m_message = std::format(format, std::forward<Args>(args)...);
    }

    const T& get_value() const { return m_value; }

    const std::string& get_message() const { return m_message; }

private:
    T m_value;
    std::string m_message;
};

#define VCT_ERROR(VALUE, ...) std::unexpected(::vct::Error(__FILE__, __FUNCTION__, __LINE__, VALUE, ##__VA_ARGS__))
;
const char* error_to_string(ErrorCode err);

}  // namespace vct
