cmake_minimum_required(VERSION 3.0)
project(VCT)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# glfw
message("Configuring glfw...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# glad
message("Configuring glad...")
add_subdirectory(external/glad)

# glm
message("Configuring glm...")
include_directories(external/glm/)

# three.math
message("Configuring three.math...")
add_subdirectory(external/three.math)
include_directories(${PROJECT_SOURCE_DIR}/external/three.math/include/)

add_definitions(-DDATA_DIR="${CMAKE_SOURCE_DIR}/data/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(source)
