cmake_minimum_required(VERSION 3.21)

project(VES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SRC_FILES ${PROJECT_NAME}/*.cpp ${PROJECT_NAME}/*.hpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

if(DEFINED VCPKG_ROOT)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
elseif(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
else()
    message(FATAL_ERROR "Please set VCPKG_ROOT (environment or cmake) to your vcpkg installation")
endif()

find_package(raylib 3.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} raylib)
