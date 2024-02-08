#if defined(EDITOR_COMMAND_DECLARE)
#define EDITOR_COMMAND(ALIAS, NAME, PARAMS, DESC, FUNC, USER_DATA) \
    ::vct::CommandLineOption{                                      \
        .name = NAME,                                              \
        .alias = ALIAS,                                            \
        .params = PARAMS,                                          \
        .func = (::vct::CommandLineFunc)FUNC,                      \
        .user_data = USER_DATA,                                    \
    },
#elif defined(EDITOR_COMMAND_HELP_FUNC)
#define EDITOR_COMMAND(ALIAS, NAME, PARAMS, DESC, FUNC, USER_DATA) \
    ::vct::print_impl(::vct::LOG_LEVEL_NORMAL, "{}\n", editor_command_help_option(ALIAS, NAME, PARAMS, DESC));
#else
#error "please define one macro"
#define EDITOR_COMMAND(...)
#endif

EDITOR_COMMAND("-h", "--help", "", "Display this help message.", editor_command_help, nullptr)
EDITOR_COMMAND("-v", "--verbose", "", "Use verbose stdout mode.", nullptr, &DVAR_verbose)
EDITOR_COMMAND("", "--rendering-driver", "<driver>", "Select rendering driver.", nullptr, &DVAR_r_backend)
EDITOR_COMMAND("", "--resolution", "<W> <H>", "Request window resolution.", nullptr, &DVAR_window_resolution)
EDITOR_COMMAND("", "--position", "<X> <Y>", "Request window position.", nullptr, &DVAR_window_position)
EDITOR_COMMAND("", "--path", "<directory>", "Path to a project <directory>", nullptr, &DVAR_scene)
EDITOR_COMMAND("", "--gpu-validation", "", "Enable graphics API validation layers.", nullptr, &DVAR_r_gpu_validation)
EDITOR_COMMAND("", "--set", "<name> <value1> <value2> ...", "Set dynamic variable.", nullptr, nullptr)

#undef EDITOR_COMMAND
