file(GLOB SRC_CONTAINER
    "Container/*.h"
)

file(GLOB SRC_CORE
    "Core/*.h"
    "Core/*.cpp"
)

file(GLOB SRC_GRAPHICS
    "Graphics/*.h"
    "Graphics/*.cpp"
)

file(GLOB SRC_MATH
    "Math/*.h"
    "Math/*.cpp"
)

file(GLOB SRC_SCENE
    "Scene/*.h"
    "Scene/*.cpp"
)

set(LIB_NAME Engine)

add_library(${LIB_NAME} STATIC
    ${SRC_CONTAINER}
    ${SRC_CORE}
    ${SRC_GRAPHICS}
    ${SRC_MATH}
    ${SRC_SCENE}
)

source_group("Container"    FILES ${SRC_CONTAINER})
source_group("Core"         FILES ${SRC_CORE})
source_group("Graphics"     FILES ${SRC_GRAPHICS})
source_group("Math"     FILES ${SRC_MATH})
source_group("Scene"        FILES ${SRC_SCENE})

target_include_directories(${LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/External/assimp/include
    ${PROJECT_SOURCE_DIR}/External/fmt/include
    ${PROJECT_SOURCE_DIR}/External/glfw/include
    ${PROJECT_SOURCE_DIR}/External/glad/include
    ${PROJECT_SOURCE_DIR}/External/glm
    ${PROJECT_SOURCE_DIR}/External/bullet
    ${PROJECT_SOURCE_DIR}/External/imgui
    ${PROJECT_SOURCE_DIR}/External
    ${PROJECT_SOURCE_DIR}/Source/Engine/Core
    ${PROJECT_SOURCE_DIR}/Source/Engine
    ${PROJECT_SOURCE_DIR}/Source/
)

target_compile_definitions(${LIB_NAME} PUBLIC
    -D_CRT_SECURE_NO_WARNINGS
    -DROOT_FOLDER="${CMAKE_SOURCE_DIR}/"
)

target_link_libraries(${LIB_NAME} PUBLIC imgui glfw glad lua fmt)

if(MSVC)
    target_compile_options(${LIB_NAME} PRIVATE /W4)
else()
    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# @TODO: move to plugin
if(MSVC)
    set(LIB_ASSIMP_DEBUG_DIR ${PROJECT_SOURCE_DIR}/External/assimp/build/lib/Debug)
    target_link_libraries(${LIB_NAME} PUBLIC debug
        ${LIB_ASSIMP_DEBUG_DIR}/assimp-vc142-mtd.lib
        ${LIB_ASSIMP_DEBUG_DIR}/IrrXMLd.lib
        ${LIB_ASSIMP_DEBUG_DIR}/zlibstaticd.lib)

    set(LIB_ASSIMP_RELEASE_DIR ${PROJECT_SOURCE_DIR}/External/assimp/build/lib/Release)
    target_link_libraries(${LIB_NAME} PUBLIC optimized
        ${LIB_ASSIMP_RELEASE_DIR}/assimp-vc142-mt.lib
        ${LIB_ASSIMP_RELEASE_DIR}/IrrXML.lib
        ${LIB_ASSIMP_RELEASE_DIR}/zlibstatic.lib)
endif()
