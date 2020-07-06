add_subdirectory(base)
add_subdirectory(imgui)
add_subdirectory(application)

add_executable(vct
    imgui_impl_opengl3.cpp
    main.cpp
)

target_link_libraries(vct
    three::math
    vct::base
    vct::app
    vct::imgui
    glad
)

target_include_directories(vct PRIVATE
    ${PROJECT_SOURCE_DIR}/external/glad/include/
    ${PROJECT_SOURCE_DIR}/source/
)
