add_subdirectory(Engine)

add_executable(vct
    dummy.cpp
)

target_link_libraries(vct PUBLIC
    Engine
)
