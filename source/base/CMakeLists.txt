add_library(base
    Exception.cpp
)

target_include_directories(base PUBLIC
    ${PROJECT_SOURCE_DIR}/external/three.math/include/
)

target_link_libraries(base PRIVATE
    three::math
)

add_library(vct::base ALIAS base)
