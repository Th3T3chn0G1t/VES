cmake_minimum_required(VERSION 3.21)

project(VES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SRC_FILES ${PROJECT_NAME}/*.cpp ${PROJECT_NAME}/*.hpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

find_package(raylib 3.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} raylib)
