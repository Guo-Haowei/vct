add_library(scene
    SceneLoader.cpp
)

target_include_directories(scene PRIVATE
    ${PROJECT_SOURCE_DIR}/source/
    ${PROJECT_SOURCE_DIR}/external/assimp/include
)

# todo: compatible with both vs2017 and 2019
set(lib_assimp_release "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Release/assimp-vc142-mt.lib")
set(lib_irrxmld_release "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Release/IrrXML.lib")
set(lib_zlib_release "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Release/zlibstatic.lib")
target_link_libraries(scene optimized ${lib_assimp_release} ${lib_irrxmld_release} ${lib_zlib_release})

set(lib_assimp_debug "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Debug/assimp-vc142-mtd.lib")
set(lib_irrxmld_debug "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Debug/IrrXMLd.lib")
set(lib_zlib_debug "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Debug/zlibstaticd.lib")
target_link_libraries(scene debug ${lib_assimp_debug} ${lib_irrxmld_debug} ${lib_zlib_debug})

add_library(vct::scene ALIAS scene)
