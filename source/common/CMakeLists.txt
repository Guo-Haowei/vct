add_library(common
    Window.cpp
    com_cmdline.cpp
    com_filesystem.cpp
    com_system.cpp
    main.cpp
)

target_link_libraries(common PRIVATE glfw)

target_include_directories(common PRIVATE
    ${PROJECT_SOURCE_DIR}/external/glfw/include
    ${PROJECT_SOURCE_DIR}/source/
)
