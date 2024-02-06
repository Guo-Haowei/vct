#include "EditorLayer.h"
#include "Framework/UIManager.h"
#include "core/os/os.h"
#include "core/os/threads.h"
#include "core/systems/job_system.h"
#include "core/utility/command_line.h"
#include "servers/display_server_glfw.h"

#define DEFINE_DVAR
#include "core/dynamic_variable/common_dvars.h"

static void register_common_dvars() {
#define REGISTER_DVAR
#include "core/dynamic_variable/common_dvars.h"
}

////////////////////////////////////////////////////////////
class Editor : public Application {
public:
    Editor() {
        AddLayer(std::make_shared<EditorLayer>());
    }
};

////////////////////////////////////////////////////////////

static std::string editor_command_help_option(std::string_view alias, std::string_view name, std::string_view param, std::string_view desc) {
    std::string result;
    result.reserve(128);
    result.append("    ");
    if (!alias.empty()) {
        result.append(alias);
        result.append(", ");
    }
    result.append(name);
    result.push_back(' ');
    result.append(param);

    while (result.length() < 40) {
        result.push_back(' ');
    }

    result.append(desc);
    return result;
}

static bool editor_command_help(void*, std::span<const char*>) {
    using vct::print_impl;

    vct::print_impl(vct::LOG_LEVEL_NORMAL, "Usage: editor.exe [options]\n");
    vct::print_impl(vct::LOG_LEVEL_NORMAL, "Options:\n");
#define EDITOR_COMMAND_HELP_FUNC
#include "editor_command.inl.h"
#undef EDITOR_COMMAND_HELP_FUNC
    std::exit(0);
}

static void process_command_line(int argc, const char** argv) {
    std::vector<vct::CommandLineOption> editor_cmdline_desc = {
#define EDITOR_COMMAND_DECLARE
#include "editor_command.inl.h"
#undef EDITOR_COMMAND_DECLARE
    };

    vct::CommandLineParser parser;
    parser.init(editor_cmdline_desc);

    std::vector<const char*> args(argv + 1, argv + argc);
    auto result = parser.process_command_line(args);
    if (!result) {
        vct::print_impl(vct::LOG_LEVEL_ERROR, "error: {}, aborting...\n", result.error());
        std::exit(-1);
    }
}

// @TODO: init os properly
static vct::OS s_os;
static vct::DisplayServer* s_display_server = new vct::DisplayServerGLFW;

int main(int argc, const char** argv) {
    OS::singleton().initialize();

    register_common_dvars();
    DynamicVariable::deserialize();
    process_command_line(argc, argv);

    thread::initialize();
    jobsystem::initialize();

    UIManager::initialize();
    DisplayServer::singleton().initialize();

    Editor editor;
    editor.Run(argc, argv);

    thread::request_shutdown();

    DisplayServer::singleton().finalize();
    UIManager::finalize();

    jobsystem::finalize();
    thread::finailize();

    DynamicVariable::serialize();

    OS::singleton().finalize();

    return 0;
}
