cmake_minimum_required(VERSION 3.8)
project(VCT)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

message("***********************************************************************")
message(STATUS "CMAKE_CXX_COMPILER_ID   : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_BUILD_TOOL        : ${CMAKE_BUILD_TOOL}")

################################################################################
message("Configuring glfw...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw)
set_target_properties(glfw PROPERTIES FOLDER thirdparty)
set_target_properties(update_mappings PROPERTIES FOLDER thirdparty)

################################################################################
message("Configuring glad...")
add_subdirectory(thirdparty/glad)
set_target_properties(glad PROPERTIES FOLDER thirdparty)

################################################################################
message(STATUS "Configuring fmt...")
add_subdirectory(thirdparty/fmt)
set_target_properties(fmt PROPERTIES FOLDER thirdparty)

################################################################################
message(STATUS "Configuring Bullet3...")
add_subdirectory(thirdparty/bullet3)
set_target_properties(bullet PROPERTIES FOLDER thirdparty)

include(cmake/build-lua.cmake)
include(cmake/build-imgui.cmake)

# gtest
enable_testing()
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/googletest)
set_target_properties(gtest PROPERTIES FOLDER thirdparty)
set_target_properties(gtest_main PROPERTIES FOLDER thirdparty)

function(target_set_warning_level TARGET)
    if(MSVC)
        target_compile_options(${TARGET} PRIVATE /W4 /WX)
    else()
        message(FATAL_ERROR "platform not supported")
    endif()
endfunction()

add_subdirectory(source)
