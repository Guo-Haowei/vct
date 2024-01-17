set(TARGET_NAME engine.tests)

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC})

add_executable(${TARGET_NAME} ${SRC})

target_include_directories(${TARGET_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/thirdparty/doctest
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(${TARGET_NAME} PRIVATE engine)

target_precompile_headers(${TARGET_NAME} PRIVATE pch.h)
