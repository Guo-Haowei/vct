#if defined(EDITOR_COMMAND_DECLARE)
#define EDITOR_COMMAND(ALIAS, NAME, PARAMS, DESC, FUNC, USER_DATA) \
    vct::CommandLineOption{                                        \
        .name = NAME,                                              \
        .alias = ALIAS,                                            \
        .params = PARAMS,                                          \
        .func = FUNC,                                              \
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
EDITOR_COMMAND("-v", "--verbose", "", "Use verbose stdout mode.", command_line_set_dvar_func, &DVAR_verbose)
EDITOR_COMMAND("", "--rendering-driver", "<driver>", "Select rendering driver.", command_line_set_dvar_func, &DVAR_r_backend)
EDITOR_COMMAND("", "--resolution", "<W> <H>", "Request window resolution.", command_line_set_dvar_func, &DVAR_window_resolution)
EDITOR_COMMAND("", "--position", "<X> <Y>", "Request window position.", command_line_set_dvar_func, &DVAR_window_position)
EDITOR_COMMAND("", "--path", "<directory>", "Path to a project <directory>", command_line_set_dvar_func, &DVAR_scene)
EDITOR_COMMAND("", "--gpu-validation", "", "Enable graphics API validation layers for debugging.", command_line_set_dvar_func, &DVAR_r_gpu_validation)

#undef EDITOR_COMMAND
