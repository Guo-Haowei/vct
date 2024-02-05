#pragma once
#include "core/dynamic_variable/dynamic_variable.h"

namespace vct {

enum ParseError {
    OUT_OF_BOUND,
    INVALID_COMMAND,
};

using CommandLineFunc = bool (*)(void* user_data, std::span<const char*> command_line);

struct CommandLineOption {
    std::string_view name;
    std::string_view alias;
    std::string_view params;
    CommandLineFunc func;
    void* user_data;
};

class CommandLineParser {
public:
    using CommandLineMap = std::map<std::string_view, CommandLineOption>;
    using CommandLineList = std::vector<CommandLineOption>;
    using CommandLine = std::vector<const char*>;
    using CommandLineAliasMap = std::unordered_map<std::string_view, std::string_view>;

    void init(const CommandLineList& command_list);

    auto process_command_line(const CommandLine& command_line) -> std::expected<void, std::string>;

private:
    auto process_next_command() -> std::expected<void, std::string>;

    auto peek() const -> std::expected<std::string_view, ParseError>;
    auto consume() -> std::expected<std::string_view, ParseError>;
    auto get_args(uint32_t count) -> std::expected<std::span<const char*>, ParseError>;

    CommandLineMap m_command_line_map;
    CommandLineAliasMap m_command_line_alias_map;
    size_t m_cursor;
    CommandLine m_command_line;
};

bool command_line_set_dvar_func(void* user_data, std::span<const char*> commands);

}  // namespace vct
