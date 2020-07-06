cmake_minimum_required(VERSION 3.0)
project(VCT)
set(CMAKE_C_STANDARD 17)
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

# stb
# message("Configuring stb...")
# include_directories(external/stb/)

# assimp
# set(lib_assimp_debug "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Debug/assimp-vc142-mtd.lib" CACHE STRING "assimp Debug")
# set(lib_irrxmld_debug "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Debug/IrrXMLd.lib" CACHE STRING "IrrXMLd Debug")
# set(lib_zlib_debug "${PROJECT_SOURCE_DIR}/external/assimp/build/lib/Debug/zlibstaticd.lib" CACHE STRING "zlib static Debug")

# rapid json
# include_directories(external/rapidjson/include)

# add_definitions(-DSHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/")
# add_definitions(-DMODEL_DIR="${CMAKE_SOURCE_DIR}/models/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(source)
