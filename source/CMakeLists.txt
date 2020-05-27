add_subdirectory(GL)

add_executable(VoxelGI
    main.cpp
    App.cpp
)

target_link_libraries(VoxelGI PRIVATE glfw glad lib_gl)
