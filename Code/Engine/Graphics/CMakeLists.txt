add_library(renderer
    gl_utils.cpp
    GpuTexture.cpp
    MainRenderer.cpp
    imgui_impl_opengl3.cpp
    r_cbuffers.cpp
    r_editor.cpp
    r_passes.cpp
    r_graphics.cpp
    r_rendertarget.cpp
    r_shader.cpp
    r_sun_shadow.cpp
)

target_link_libraries(renderer PRIVATE
    glad
)

target_include_directories(renderer PUBLIC
    ${PROJECT_SOURCE_DIR}/source/
    ${PROJECT_SOURCE_DIR}/external/glad/include/
    ${PROJECT_SOURCE_DIR}/external/stb/
    ${PROJECT_SOURCE_DIR}/external/imgui/
)

add_library(vct::renderer ALIAS renderer)
