cmake_minimum_required(VERSION 3.0)
project(VoxelGI)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# glfw
message("Configuring glfw...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
include_directories(external/glfw/include)
add_subdirectory(external/glfw)

# glad
message("Configuring glad...")
include_directories(external/glad/include)
add_subdirectory(external/glad)

add_definitions(-DSHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(source)
