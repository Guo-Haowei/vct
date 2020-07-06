add_subdirectory(base)
add_subdirectory(imgui)
add_subdirectory(application)
add_subdirectory(renderer)

add_executable(vct
    main.cpp
)

target_link_libraries(vct
    three::math
    vct::base
    vct::app
    vct::imgui
    vct::renderer
)
