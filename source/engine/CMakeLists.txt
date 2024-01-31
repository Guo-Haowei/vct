set(LIB_NAME engine)

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${LIB_NAME} STATIC ${SRC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/engine FILES ${SRC})

target_include_directories(${LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/thirdparty/assimp/include
    ${PROJECT_SOURCE_DIR}/thirdparty/bullet
    ${PROJECT_SOURCE_DIR}/thirdparty/glfw/include
    ${PROJECT_SOURCE_DIR}/thirdparty/glad/include
    ${PROJECT_SOURCE_DIR}/thirdparty/glm
    ${PROJECT_SOURCE_DIR}/thirdparty/imgui
    ${PROJECT_SOURCE_DIR}/thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
    # @TODO: fix include
    ${CMAKE_CURRENT_SOURCE_DIR}/../shaders
)

target_compile_definitions(${LIB_NAME} PUBLIC
    -D_CRT_SECURE_NO_WARNINGS
    -DROOT_FOLDER="${PROJECT_SOURCE_DIR}/"
)

target_link_libraries(${LIB_NAME} PUBLIC imgui glfw glad lua bullet)

target_compile_options(${LIB_NAME} PRIVATE /W4)

target_precompile_headers(${LIB_NAME} PRIVATE src/engine/pch.h)

# @TODO: move to plugin
if(MSVC)
    set(LIB_ASSIMP_DEBUG_DIR ${PROJECT_SOURCE_DIR}/thirdparty/assimp/build/lib/Debug)
    target_link_libraries(${LIB_NAME} PUBLIC debug
        ${LIB_ASSIMP_DEBUG_DIR}/assimp-vc143-mtd.lib
        ${LIB_ASSIMP_DEBUG_DIR}/IrrXMLd.lib
        ${LIB_ASSIMP_DEBUG_DIR}/zlibstaticd.lib)

    set(LIB_ASSIMP_RELEASE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/assimp/build/lib/Release)
    target_link_libraries(${LIB_NAME} PUBLIC optimized
        ${LIB_ASSIMP_RELEASE_DIR}/assimp-vc143-mt.lib
        ${LIB_ASSIMP_RELEASE_DIR}/IrrXML.lib
        ${LIB_ASSIMP_RELEASE_DIR}/zlibstatic.lib)
endif()

add_subdirectory(tests)
