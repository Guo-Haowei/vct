cmake_minimum_required(VERSION 3.0)
project(VCT)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

message("***********************************************************************")
message(STATUS "CMAKE_CXX_COMPILER_ID   : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_BUILD_TOOL        : ${CMAKE_BUILD_TOOL}")

################################################################################
message("Configuring glfw...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(External/glfw)

################################################################################
message("Configuring glad...")
add_subdirectory(External/glad)
set_target_properties(glad PROPERTIES FOLDER External)

################################################################################
message(STATUS "Configuring fmt...")
add_subdirectory(External/fmt)
set_target_properties(fmt PROPERTIES FOLDER External)

################################################################################
message(STATUS "Configuring Bullet3...")
add_subdirectory(External/bullet3)
set_target_properties(bullet PROPERTIES FOLDER External)

include(cmake/build-lua.cmake)
include(cmake/build-imgui.cmake)

add_subdirectory(Source)
add_subdirectory(Test)
