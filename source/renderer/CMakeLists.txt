add_library(renderer
    gl_utils.cpp
    GlslProgram.cpp
    GpuTexture.cpp
    RenderTarget.cpp
    MainRenderer.cpp
    imgui_impl_opengl3.cpp
    r_cbuffers.cpp
    r_editor.cpp
    r_graphics.cpp
    r_sun_shadow.cpp
)

target_link_libraries(renderer PRIVATE
    glad
)

target_include_directories(renderer PUBLIC
    ${PROJECT_SOURCE_DIR}/source/
    ${PROJECT_SOURCE_DIR}/external/glad/include/
    ${PROJECT_SOURCE_DIR}/external/stb/
)

add_library(vct::renderer ALIAS renderer)
