cmake_minimum_required(VERSION 3.21)

project(VES)

file(GLOB_RECURSE SRC_FILES VES/*.cpp VES/*.hpp)

add_executable(VES ${SRC_FILES})