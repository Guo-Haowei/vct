#include "dynamic_variable_manager.h"

#include <sstream>

#include "core/io/archive.h"

namespace vct {

static constexpr const char* DVAR_CACHE_FILE = "@user://dynamic_variables.cache";

void DynamicVariableManager::serialize() {
    auto res = FileAccess::open(DVAR_CACHE_FILE, FileAccess::WRITE);
    if (!res) {
        LOG_ERROR("{}", res.error().get_message());
        return;
    }

    LOG("[dvar] serializing dvars");
    auto writer = std::move(*res);

    for (auto const& [key, dvar] : DynamicVariable::s_map) {
        if (dvar->m_flags & DVAR_FLAG_SERIALIZE) {
            auto line = std::format("+set {} {}\n", dvar->m_name, dvar->value_to_string());
            writer->write_buffer(line.data(), line.length());
        }
    }

    writer->close();
}

void DynamicVariableManager::deserialize() {
    auto res = FileAccess::open(DVAR_CACHE_FILE, FileAccess::READ);
    if (!res) {
        LOG_ERROR("{}", res.error().get_message());
        return;
    }

    auto reader = std::move(*res);
    const size_t size = reader->get_length();
    std::string buffer;
    buffer.resize(size);
    reader->read_buffer(buffer.data(), size);
    reader->close();

    std::stringstream ss{ buffer };
    std::vector<std::string> commands;
    std::string token;
    while (!ss.eof() && ss >> token) {
        commands.emplace_back(token);
    }

    DynamicVariableParser parser(commands, DynamicVariableParser::SOURCE_CACHE);
    if (!parser.parse()) {
        LOG_ERROR("[dvar] Error: {}", parser.get_error());
    }
}

void DynamicVariableManager::dump_dvars() {
    for (const auto& it : DynamicVariable::s_map) {
        PRINT("-- {}, '{}'", it.first, it.second->get_desc());
    }
}

//--------------------------------------------------------------------------------------------------
// Dynamic Varialbe Parser
//--------------------------------------------------------------------------------------------------

bool DynamicVariableParser::parse() {
    for (;;) {
        const std::string& command = peek();
        if (command == s_eof) {
            return true;
        }

        if (command == "+set") {
            consume();  // pop set
            if (!process_set()) {
                return false;
            }
        } else if (command == "+list") {
            consume();
            // DynamicVariableManager::dump_dvars();
        } else {
            m_error = std::format("unknown command '{}'", command);
            return false;
        }
    }
}

bool DynamicVariableParser::process_set() {
    const std::string& name = consume();
    if (name == s_eof) {
        m_error = "unexpected <EOF>";
        return false;
    }

    DynamicVariable* dvar = DynamicVariable::find_dvar(name);
    if (dvar == nullptr) {
        m_error = std::format("dvar '{}' not found", name);
        return false;
    }

    VariantType type = dvar->get_type();
    bool ok = true;
    std::string_view str;
    int ix = 0, iy = 0, iz = 0, iw = 0;
    float fx = 0, fy = 0, fz = 0, fw = 0;
    size_t arg_start_index = m_cursor;
    switch (type) {
        case VARIANT_TYPE_INT:
            ok = ok && try_get_int(ix);
            ok = ok && dvar->set_int(ix);
            break;
        case VARIANT_TYPE_FLOAT:
            ok = ok && try_get_float(fx);
            ok = ok && dvar->set_float(fx);
            break;
        case VARIANT_TYPE_STRING:
            ok = ok && try_get_string(str);
            ok = ok && dvar->set_string(str);
            break;
        case VARIANT_TYPE_VEC2:
            ok = ok && try_get_float(fx);
            ok = ok && try_get_float(fy);
            ok = ok && dvar->set_vec2(fx, fy);
            break;
        case VARIANT_TYPE_VEC3:
            ok = ok && try_get_float(fx);
            ok = ok && try_get_float(fy);
            ok = ok && try_get_float(fz);
            ok = ok && dvar->set_vec3(fx, fy, fz);
            break;
        case VARIANT_TYPE_VEC4:
            ok = ok && try_get_float(fx);
            ok = ok && try_get_float(fy);
            ok = ok && try_get_float(fz);
            ok = ok && try_get_float(fw);
            ok = ok && dvar->set_vec4(fx, fy, fz, fw);
            break;
        case VARIANT_TYPE_IVEC2:
            ok = ok && try_get_int(ix);
            ok = ok && try_get_int(iy);
            ok = ok && dvar->set_ivec2(ix, iy);
            break;
        case VARIANT_TYPE_IVEC3:
            ok = ok && try_get_int(ix);
            ok = ok && try_get_int(iy);
            ok = ok && try_get_int(iz);
            ok = ok && dvar->set_ivec3(ix, iy, iz);
            break;
        case VARIANT_TYPE_IVEC4:
            ok = ok && try_get_int(ix);
            ok = ok && try_get_int(iy);
            ok = ok && try_get_int(iz);
            ok = ok && try_get_int(iw);
            ok = ok && dvar->set_ivec4(ix, iy, iz, iw);
            break;
        default:
            CRASH_NOW();
            break;
    }

    if (!ok) {
        m_error = std::format("invalid arguments: +set {}", name);
        for (size_t i = arg_start_index; i < m_commands.size(); ++i) {
            m_error.push_back(' ');
            m_error.append(m_commands[i]);
        }
        return false;
    }

    // @TODO: refactor
    switch (m_source) {
        case SOURCE_CACHE:
            dvar->print_value_change("cache");
            break;
        case SOURCE_COMMAND_LINE:
            dvar->print_value_change("command line");
            break;
        default:
            break;
    }
    return true;
}

const std::string& DynamicVariableParser::peek() {
    if (out_of_bound()) {
        return s_eof;
    }

    return m_commands[m_cursor];
}

const std::string& DynamicVariableParser::consume() {
    if (out_of_bound()) {
        return s_eof;
    }

    return m_commands[m_cursor++];
}

bool DynamicVariableParser::try_get_int(int& out) {
    if (out_of_bound()) {
        return false;
    }
    const std::string& value = consume();
    if (value == "true") {
        out = 1;
    } else if (value == "false") {
        out = 0;
    } else {
        out = atoi(value.c_str());
    }
    return true;
}

bool DynamicVariableParser::try_get_float(float& out) {
    if (out_of_bound()) {
        return false;
    }
    out = (float)atof(consume().c_str());
    return true;
}

bool DynamicVariableParser::try_get_string(std::string_view& out) {
    if (out_of_bound()) {
        return false;
    }

    const std::string& next = consume();
    if (next.length() >= 2 && next.front() == '"' && next.back() == '"') {
        out = std::string_view(next.data() + 1, next.length() - 2);
    } else {
        out = std::string_view(next.data(), next.length());
    }

    return true;
}

bool DynamicVariableParser::out_of_bound() {
    return m_cursor >= m_commands.size();
}

bool DynamicVariableManager::parse(const std::vector<std::string>& commands) {
    DynamicVariableParser parser(commands, DynamicVariableParser::SOURCE_COMMAND_LINE);
    bool ok = parser.parse();
    if (!ok) {
        LOG_ERROR("[dvar] Error: {}", parser.get_error());
    }
    return ok;
}

}  // namespace vct