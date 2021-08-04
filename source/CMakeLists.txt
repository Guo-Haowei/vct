add_subdirectory(universal)
add_subdirectory(imgui)
add_subdirectory(common)
add_subdirectory(scene)
add_subdirectory(renderer)

add_executable(vct
    dummy.cpp
)

target_link_libraries(vct PUBLIC
    universal
    common
    imgui
    vct::renderer
    vct::scene
)
