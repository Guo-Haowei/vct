add_library(bullet STATIC
	btBulletCollisionAll.cpp
	btBulletDynamicsAll.cpp
	btLinearMathAll.cpp
)

target_include_directories(bullet PUBLIC ${CMAKE_SOURCE_DIR})
target_compile_definitions(bullet PUBLIC -D__BT_DISABLE_SSE__ -DBT_USE_DOUBLE_PRECISION)

target_compile_options(bullet PRIVATE /W0)
