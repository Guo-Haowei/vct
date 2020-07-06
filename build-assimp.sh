#!/bin/bash
cd external/assimp || exit 1
if [ ! -d build ]; then
    mkdir build || exit 1
fi
cd build || exit 1
cmake -DBUILD_SHARED_LIBS=OFF .. || exit 1
cmake --build . --config Debug || exit 1
cmake --build . --config Release || exit 1

cp include/assimp/config.h ../include/assimp/ || exit 1
