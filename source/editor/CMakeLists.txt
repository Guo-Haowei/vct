set(TARGET_NAME editor)

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${TARGET_NAME} ${SRC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/editor FILES ${SRC})

target_link_libraries(${TARGET_NAME} PUBLIC engine)

target_compile_options(${TARGET_NAME} PRIVATE /W4)

set_target_properties(${TARGET_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
