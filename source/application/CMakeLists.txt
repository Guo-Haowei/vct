add_library(app
    Window.cpp
    Application.cpp
)

target_link_libraries(app PRIVATE glfw)

target_include_directories(app PRIVATE
    ${PROJECT_SOURCE_DIR}/external/glfw/include
    ${PROJECT_SOURCE_DIR}/source/
)

add_library(vct::app ALIAS app)
