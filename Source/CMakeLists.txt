add_subdirectory(Engine)

add_executable(vct
    dummy.cpp
)

target_link_libraries(vct PUBLIC
    Engine
)

set_target_properties(vct
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
