set(TESTNAME SerializeTest)

add_executable(${TESTNAME}
    SerializeTest.cpp
)

target_include_directories(${TESTNAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/Source/Engine
    ${PROJECT_SOURCE_DIR}/thirdparty/doctest
)

target_link_libraries(${TESTNAME} PRIVATE
    Engine
)

set_target_properties(${TESTNAME} PROPERTIES FOLDER Test)