set(TARGET_NAME universal.tests)

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests FILES ${SRC})

add_executable(${TARGET_NAME} ${SRC})

target_include_directories(${TARGET_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/thirdparty/doctest
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_precompile_headers(${TARGET_NAME} PRIVATE tests/pch.h)
