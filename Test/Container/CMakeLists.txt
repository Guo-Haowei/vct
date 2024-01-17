set(TESTNAME ContainerTest)

add_executable(${TESTNAME}
    ContainerTest.cpp
)

target_include_directories(${TESTNAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/Source/Engine
    ${PROJECT_SOURCE_DIR}/thirdparty/doctest
)

set_target_properties(${TESTNAME} PROPERTIES FOLDER Test)