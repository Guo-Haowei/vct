set(LIB_NAME Engine)

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(${LIB_NAME} STATIC ${SRC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC})

target_include_directories(${LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/External/assimp/include
    ${PROJECT_SOURCE_DIR}/External/bullet
    ${PROJECT_SOURCE_DIR}/External/fmt/include
    ${PROJECT_SOURCE_DIR}/External/glfw/include
    ${PROJECT_SOURCE_DIR}/External/glad/include
    ${PROJECT_SOURCE_DIR}/External/glm
    ${PROJECT_SOURCE_DIR}/External/imgui
    ${PROJECT_SOURCE_DIR}/External
    ${PROJECT_SOURCE_DIR}/Source/Engine
    ${PROJECT_SOURCE_DIR}/Source/
)

target_compile_definitions(${LIB_NAME} PUBLIC
    -D_CRT_SECURE_NO_WARNINGS
    -DROOT_FOLDER="${PROJECT_SOURCE_DIR}/"
)

target_link_libraries(${LIB_NAME} PUBLIC imgui glfw glad lua fmt bullet)

target_compile_options(${LIB_NAME} PRIVATE /W4)

# @TODO: move to plugin
if(MSVC)
    set(LIB_ASSIMP_DEBUG_DIR ${PROJECT_SOURCE_DIR}/External/assimp/build/lib/Debug)
    target_link_libraries(${LIB_NAME} PUBLIC debug
        ${LIB_ASSIMP_DEBUG_DIR}/assimp-vc143-mtd.lib
        ${LIB_ASSIMP_DEBUG_DIR}/IrrXMLd.lib
        ${LIB_ASSIMP_DEBUG_DIR}/zlibstaticd.lib)

    set(LIB_ASSIMP_RELEASE_DIR ${PROJECT_SOURCE_DIR}/External/assimp/build/lib/Release)
    target_link_libraries(${LIB_NAME} PUBLIC optimized
        ${LIB_ASSIMP_RELEASE_DIR}/assimp-vc143-mt.lib
        ${LIB_ASSIMP_RELEASE_DIR}/IrrXML.lib
        ${LIB_ASSIMP_RELEASE_DIR}/zlibstatic.lib)
endif()
