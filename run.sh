#!/bin/sh

cmake -B build -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=vcpkg.cmake . && cmake --build build && build/VES
