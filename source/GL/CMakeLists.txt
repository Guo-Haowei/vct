add_library(lib_gl
    GpuResource.cpp
    GpuBuffer.cpp
    VertexArray.cpp
    ShaderProgram.cpp
)

target_link_libraries(lib_gl PUBLIC glad)
