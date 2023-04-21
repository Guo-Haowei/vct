set(EXE_NAME Editor)

file(GLOB SRC_EDITOR
    "*.h"
    "*.cpp"
)

add_executable(${EXE_NAME}
    ${SRC_EDITOR}
)

source_group("Editor" FILES ${SRC_EDITOR})

target_link_libraries(${EXE_NAME} PUBLIC
    Engine
)

target_compile_options(${EXE_NAME} PRIVATE /W4)

set_target_properties(${EXE_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
