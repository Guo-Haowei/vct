add_subdirectory(base)
add_subdirectory(imgui)
add_subdirectory(application)

add_executable(vct
    imgui_impl_opengl3.cpp
    main.cpp
)

target_link_libraries(vct
    vct::base
    vct::app
    vct::imgui
    glad
    glfw
)

target_include_directories(vct PRIVATE
    ${PROJECT_SOURCE_DIR}/external/glad
    ${PROJECT_SOURCE_DIR}/external/glfw
    ${PROJECT_SOURCE_DIR}/source/
)
