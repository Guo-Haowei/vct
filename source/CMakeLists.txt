# lua
add_library(lua
    lua/lapi.c
    lua/lauxlib.c
    lua/lbaselib.c
    lua/lcode.c
    lua/lcorolib.c
    lua/lctype.c
    lua/ldblib.c
    lua/ldebug.c
    lua/ldo.c
    lua/ldump.c
    lua/lfunc.c
    lua/lgc.c
    lua/linit.c
    lua/liolib.c
    lua/llex.c
    lua/lmathlib.c
    lua/lmem.c
    lua/loadlib.c
    lua/lobject.c
    lua/lopcodes.c
    lua/loslib.c
    lua/lparser.c
    lua/lstate.c
    lua/lstring.c
    lua/lstrlib.c
    lua/ltablib.c
    lua/ltable.c
    # lua/ltests.c
    lua/ltm.c
    # lua/lua.c
    lua/lundump.c
    lua/lutf8lib.c
    lua/lvm.c
    lua/lzio.c
)

add_subdirectory(universal)
add_subdirectory(imgui)
add_subdirectory(common)
add_subdirectory(scene)
add_subdirectory(renderer)

add_executable(vct
    dummy.cpp
)

target_link_libraries(vct PUBLIC
    lua
    universal
    common
    imgui
    vct::renderer
    vct::scene
)

add_executable(ssaa ssaa.cpp)
