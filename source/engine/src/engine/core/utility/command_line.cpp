#include "command_line.h"

#include <charconv>

namespace vct {

static constexpr const char* eof = "<EOF>";

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
    m_command_line.push_back(eof);

    m_cursor = 0;

    for (;;) {
        if (peek() == eof) {
            break;
        }

        auto res = process_next_command();
        if (!res) {
            return std::unexpected(res.error());
        }
    }

    return std::expected<void, std::string>();
}

CommandLineOption* CommandLineParser::find_command(std::string_view option) {
    std::string_view option2 = option;
    if (auto it = m_command_line_alias_map.find(option); it != m_command_line_alias_map.end()) {
        option2 = it->second;
    }

    // look for descriptor
    auto it = m_command_line_map.find(option2);
    if (it == m_command_line_map.end()) {
        return nullptr;
    }

    return &it->second;
}

bool CommandLineParser::process_int(DynamicVariable* dvar) {
    auto next = peek();

    if (dvar->get_type() == VARIANT_TYPE_INT) {
        int value = 0;
        if (next == eof || find_command(next) != nullptr) {
            value = 1;  // default to 1
        } else {
            consume();  // consume value
            if (next == "true") {
                value = 1;
            } else if (next == "false") {
                value = 0;
            } else {
                std::from_chars(next.data(), next.data() + next.size(), value);
            }
        }

        dvar->set_int(value);
    }

    return true;
}

bool CommandLineParser::process_dvar(DynamicVariable* dvar) {
    int expected_args = -1;
    switch (dvar->get_type()) {
        case VARIANT_TYPE_FLOAT:
        case VARIANT_TYPE_STRING:
            expected_args = 1;
            break;
        case VARIANT_TYPE_VEC2:
        case VARIANT_TYPE_IVEC2:
            expected_args = 2;
            break;
        case VARIANT_TYPE_VEC3:
        case VARIANT_TYPE_IVEC3:
            expected_args = 3;
            break;
        case VARIANT_TYPE_VEC4:
        case VARIANT_TYPE_IVEC4:
            expected_args = 4;
            break;
        default:
            CRASH_NOW();
            break;
    }

    for (int i = 0; i < expected_args; ++i) {
        auto next = consume();
        if (next == eof) {
            return false;
        }
    }

    std::span<const char*> args(m_command_line.data() + m_cursor - expected_args, expected_args);

    float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
    int ix = 0, iy = 0, iz = 0, iw = 0;
    switch (dvar->get_type()) {
        case VARIANT_TYPE_INT:
            dvar->set_int(atoi(args[0]));
            break;
        case VARIANT_TYPE_FLOAT:
            dvar->set_float(static_cast<float>(atof(args[0])));
            break;
        case VARIANT_TYPE_STRING:
            dvar->set_string(args[0]);
            break;
        case VARIANT_TYPE_VEC4:
            w = static_cast<float>(atof(args[3]));
            [[fallthrough]];
        case VARIANT_TYPE_VEC3:
            z = static_cast<float>(atof(args[2]));
            [[fallthrough]];
        case VARIANT_TYPE_VEC2:
            y = static_cast<float>(atof(args[1]));
            x = static_cast<float>(atof(args[0]));
            break;
        case VARIANT_TYPE_IVEC4:
            iw = atoi(args[3]);
            [[fallthrough]];
        case VARIANT_TYPE_IVEC3:
            iz = atoi(args[2]);
            [[fallthrough]];
        case VARIANT_TYPE_IVEC2:
            iy = atoi(args[1]);
            ix = atoi(args[0]);
            break;
        default:
            CRASH_NOW_MSG("TODO: ERROR HANDLING");
            break;
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
        case VARIANT_TYPE_IVEC2:
            dvar->set_ivec2(ix, iy);
            break;
        case VARIANT_TYPE_IVEC3:
            dvar->set_ivec3(ix, iy, iz);
            break;
        case VARIANT_TYPE_IVEC4:
            dvar->set_ivec4(ix, iy, iz, iw);
            break;
        default:
            break;
    }

    return true;
}

// @TODO: better error handling
auto CommandLineParser::process_next_command() -> std::expected<void, std::string> {
    auto option = consume();
    DEV_ASSERT(option != eof);

    const CommandLineOption* command = find_command(option);

    if (!command) {
        return std::unexpected(std::format("unrecognized command line option '{}'", option));
    }

    if (command->func) {
        command->func(command->user_data);
        return std::expected<void, std::string>();
    }

    DynamicVariable* dvar = nullptr;
    if (command->name == "--set") {
        std::string_view variable_name = consume();
        dvar = DynamicVariable::find_dvar(std::string(variable_name));
    } else {
        dvar = static_cast<DynamicVariable*>(command->user_data);
    }

    DEV_ASSERT(dvar);

    if (dvar->get_type() == VARIANT_TYPE_INT) {
        process_int(dvar);
    } else {
        process_dvar(dvar);
    }

    dvar->unset_flag(DVAR_FLAG_DESERIALIZE);
    dvar->print_value_change("command line");
    return std::expected<void, std::string>();
}

std::string_view CommandLineParser::peek() {
    if (m_cursor >= m_command_line.size()) {
        return "";
    }

    return m_command_line.at(m_cursor);
}

std::string_view CommandLineParser::consume() {
    if (m_cursor >= m_command_line.size()) {
        return "";
    }

    return m_command_line.at(m_cursor++);
}

}  // namespace vct
