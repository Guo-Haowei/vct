add_library(common
    camera.cpp
    com_cmdline.cpp
    com_filesystem.cpp
    com_misc.cpp
    editor.cpp
    geometry.cpp
    imgui_impl_glfw.cpp
    lua_script.cpp
    main_window.cpp
    main.cpp
)

target_link_libraries(common PRIVATE glfw)

target_include_directories(common PRIVATE
    ${PROJECT_SOURCE_DIR}/external/glfw/include
    ${PROJECT_SOURCE_DIR}/external/imgui/
    ${PROJECT_SOURCE_DIR}/source/
)
