#include "command_line.h"

#include <charconv>

namespace vct {

void CommandLineParser::init(const CommandLineList& command_list) {
    for (const auto& it : command_list) {
        assert(m_command_line_map.find(it.name) == m_command_line_map.end());
        m_command_line_map[it.name] = it;
        if (!it.alias.empty()) {
            assert(m_command_line_alias_map.find(it.alias) == m_command_line_alias_map.end(0));
            m_command_line_alias_map[it.alias] = it.name;
        }
    }
}

auto CommandLineParser::process_command_line(const CommandLine& command_line) -> std::expected<void, std::string> {
    m_command_line = std::move(command_line);
    m_cursor = 0;

    for (;;) {
        if (!peek()) {
            break;
        }

        auto res = process_next_command();
        if (!res) {
            return std::unexpected(res.error());
        }
    }

    return std::expected<void, std::string>();
}

auto CommandLineParser::process_next_command() -> std::expected<void, std::string> {
    auto res = peek();
    assert(res.has_value());

    // look for alias first
    std::string_view option = *res;
    consume();

    if (auto it = m_command_line_alias_map.find(option); it != m_command_line_alias_map.end()) {
        option = it->second;
    }

    // look for descriptor
    auto it = m_command_line_map.find(option);
    if (it == m_command_line_map.end()) {
        return std::unexpected(std::format("unrecognized command line option '{}'", option));
    }

    const CommandLineOption& command = it->second;

    // get arg count
    uint32_t arg_count = 0;
    int angle_bracket = 0;

    for (char c : command.params) {
        if (c == '<') {
            ++angle_bracket;
            ++arg_count;
        } else if (c == '>') {
            --angle_bracket;
        }
    }
    DEV_ASSERT(angle_bracket == 0);

    auto args = get_args(arg_count);
    if (!args) {
        return std::unexpected("not enough arguments provided");
    }

    auto result = command.func(command.user_data, *args);
    if (!result) {
        return std::unexpected("error executing");
    }

    return std::expected<void, std::string>();
}

auto CommandLineParser::peek() const -> std::expected<std::string_view, ParseError> {
    if (m_cursor >= m_command_line.size()) {
        return std::unexpected(ParseError::OUT_OF_BOUND);
    }

    return m_command_line.at(m_cursor);
}

auto CommandLineParser::consume() -> std::expected<std::string_view, ParseError> {
    if (m_cursor >= m_command_line.size()) {
        return std::unexpected(ParseError::OUT_OF_BOUND);
    }

    return m_command_line.at(m_cursor++);
}

auto CommandLineParser::get_args(uint32_t count) -> std::expected<std::span<const char*>, ParseError> {
    const char** begin = m_command_line.data() + m_cursor;
    for (uint32_t i = 0; i < count; ++i) {
        if (auto res = consume(); !res) {
            return std::unexpected(res.error());
        }
    }
    return std::span<const char*>(begin, count);
}

bool command_line_set_dvar_func(void* user_data, std::span<const char*> command_line) {
    DynamicVariable* dvar = reinterpret_cast<DynamicVariable*>(user_data);
    DEV_ASSERT(dvar);

    if (command_line.empty()) {
        DEV_ASSERT(dvar->get_type() == VARIANT_TYPE_INT);
        dvar->set_int(1);
        return true;
    }

    float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
    switch (dvar->get_type()) {
        case VARIANT_TYPE_INT:
            dvar->set_int(atoi(command_line[0]));
            break;
        case VARIANT_TYPE_FLOAT:
            dvar->set_float(static_cast<float>(atof(command_line[0])));
            break;
        case VARIANT_TYPE_STRING:
            dvar->set_string(command_line[0]);
            break;
        case VARIANT_TYPE_VEC4:
            w = static_cast<float>(atof(command_line[3]));
            [[fallthrough]];
        case VARIANT_TYPE_VEC3:
            z = static_cast<float>(atof(command_line[2]));
            [[fallthrough]];
        case VARIANT_TYPE_VEC2:
            y = static_cast<float>(atof(command_line[1]));
            x = static_cast<float>(atof(command_line[0]));
            break;
        default:
            // @TODO:
            DEV_ASSERT(0 && "TODO: ERROR HANDLING");
            return false;
    }

    switch (dvar->get_type()) {
        case VARIANT_TYPE_VEC2:
            dvar->set_vec2(x, y);
            break;
        case VARIANT_TYPE_VEC3:
            dvar->set_vec3(x, y, z);
            break;
        case VARIANT_TYPE_VEC4:
            dvar->set_vec4(x, y, z, w);
            break;
        default:
            break;
    }

    return true;
}

}  // namespace vct
