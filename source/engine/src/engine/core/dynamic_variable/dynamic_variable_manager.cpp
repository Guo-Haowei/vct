#include "dynamic_variable_manager.h"

#include "common_dvars.h"
#include "core/io/archive.h"

namespace vct {

static constexpr const char* DVAR_CACHE_FILE = "dynamic_variables.cache";

void DynamicVariableManager::serialize() {
    Archive writer;
    if (auto res = writer.open_write(DVAR_CACHE_FILE); !res) {
        LOG_ERROR("{}", res.error().get_message());
        return;
    }

    LOG("[dvar] serializing dvars");

    std::vector<DynamicVariable*> dvars;
    dvars.reserve(DynamicVariable::s_map.size());

    for (auto const& [key, dvar] : DynamicVariable::s_map) {
        if (dvar->m_flags & DVAR_FLAG_SERIALIZE) {
            dvars.push_back(dvar);
        }
    }

    size_t count = dvars.size();

    writer << count;
    for (DynamicVariable* dvar : dvars) {
        writer << dvar->m_debug_name;
        writer << dvar->m_string;
        writer.write(dvar->m_vec);
        // @TODO: better infomation
        LOG_VERBOSE("dvar {} serialized.", dvar->m_debug_name);
    }
}

void DynamicVariableManager::deserialize() {
    if (DVAR_GET_BOOL(delete_dvar_cache)) {
        std::filesystem::remove(DVAR_CACHE_FILE);
        return;
    }

    Archive reader;
    if (auto res = reader.open_read(DVAR_CACHE_FILE); !res) {
        if (res.error().get_value() != ERR_FILE_NOT_FOUND) {
            LOG_ERROR("{}", res.error().get_message());
        }
        return;
    }

    LOG("[dvar] deserializing dvars");

    size_t count = 0;

    reader >> count;
    for (size_t i = 0; i < count; ++i) {
        std::string debug_name;
        reader >> debug_name;

        DynamicVariable* dvar = DynamicVariable::find_dvar(debug_name);
        if (!dvar) {
            reader.close();
            std::filesystem::remove(DVAR_CACHE_FILE);
            return;
        }

        if (dvar->m_flags & DVAR_FLAG_DESERIALIZE) {
            reader >> dvar->m_string;
            reader.read(dvar->m_vec);
            dvar->print_value_change("cache");
        }
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
            ok = ok && !out_of_bound();
            ok = ok && dvar->set_string(consume());
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

    dvar->unset_flag(DVAR_FLAG_DESERIALIZE);
    dvar->print_value_change("command line");
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

bool DynamicVariableParser::out_of_bound() {
    return m_cursor >= m_commands.size();
}

bool DynamicVariableManager::parse(const std::vector<std::string>& commands) {
    DynamicVariableParser parser(commands);
    bool ok = parser.parse();
    if (!ok) {
        LOG_ERROR("[dvar] Error: {}", parser.get_error());
    }
    return ok;
}

}  // namespace vct