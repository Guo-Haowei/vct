add_library(lib_scene_loader
    AssimpLoader.cpp
    MyLoader.cpp
)

target_link_libraries(lib_scene_loader
    PUBLIC
    ${lib_assimp_debug}
    ${lib_irrxmld_debug}
    ${lib_zlib_debug}
)

target_include_directories(lib_scene_loader
    PRIVATE
    ${PROJECT_SOURCE_DIR}/external/assimp/include
)
