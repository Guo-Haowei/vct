set(EXE_NAME Editor)

add_executable(${EXE_NAME}
    Editor.cpp
    main.cpp
)

target_link_libraries(${EXE_NAME} PUBLIC
    Engine
)

set_target_properties(${EXE_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
