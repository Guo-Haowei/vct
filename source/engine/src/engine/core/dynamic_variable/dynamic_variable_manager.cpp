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

        reader >> dvar->m_string;
        reader.read(dvar->m_vec);

        if (dvar->m_flags & DVAR_FLAG_DESERIALIZE) {
            // @TODO: better infomation
            dvar->print_value_change("cache");
        }
    }
}

}  // namespace vct