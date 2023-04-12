add_subdirectory(universal)
add_subdirectory(common)
add_subdirectory(scene)
add_subdirectory(renderer)

add_executable(vct
    dummy.cpp
)

target_link_libraries(vct PUBLIC
    lua
    universal
    common
    imgui
    vct::renderer
    vct::scene
)

# add_executable(ssaa ssaa.cpp)
