#pragma once
#include "dynamic_variable.h"

namespace vct {

//--------------------------------------------------------------------------------------------------
// Dynamic Varialbe Parser
//--------------------------------------------------------------------------------------------------
class DynamicVariableParser {
public:
    enum Source {
        SOURCE_NONE,
        SOURCE_COMMAND_LINE,
        SOURCE_CACHE,
    };

    DynamicVariableParser(const std::vector<std::string>& commands, Source source)
        : m_source(source),
          m_commands(commands) {}

    bool parse();

    const std::string& get_error() const { return m_error; }

private:
    bool process_set();

    bool out_of_bound();
    const std::string& peek();
    const std::string& consume();

    bool try_get_int(int& out);
    bool try_get_float(float& out);
    bool try_get_string(std::string_view& out);

    const Source m_source;
    const std::vector<std::string>& m_commands;
    size_t m_cursor = 0;
    inline static const std::string s_eof = "<EOF>";
    std::string m_error;
};

class DynamicVariableManager {
public:
    static void serialize();
    static void deserialize();
    static bool parse(const std::vector<std::string>& commands);
    static void dump_dvars();
};

}  // namespace vct
