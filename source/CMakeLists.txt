add_subdirectory(GL)
add_subdirectory(math)
add_subdirectory(loaders)

add_executable(VoxelGI
    main.cpp
    App.cpp
    SceneManager.cpp
    VoxelPass.cpp
    Camera.cpp
    MainPass.cpp
    VisualizationPass.cpp
    TextureRenderer.cpp
)

target_link_libraries(VoxelGI
    PRIVATE
    glfw
    glad
    lib_geomath
    lib_gl
    lib_scene_loader
)

add_executable(convert
    assimp2json.cpp
    App.cpp
    SceneManager.cpp
    VoxelPass.cpp
    Camera.cpp
    MainPass.cpp
    VisualizationPass.cpp
    TextureRenderer.cpp
)

target_link_libraries(convert
    PRIVATE
    glfw
    glad
    lib_geomath
    lib_gl
    lib_scene_loader
)
