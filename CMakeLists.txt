cmake_minimum_required(VERSION 3.18.1)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0111 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

include(vcpkg.cmake)

project(VES)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.hpp src/*.c src/*.h)
add_executable(${PROJECT_NAME} ${SRC_FILES})

set(raylib_USE_STATIC_LIBS ON)
find_package(raylib CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_path(LUAJIT_INCLUDE_DIR luajit.h PATH_SUFFIXES luajit luajit-2.0 luajit-2.1)
find_library(LUAJIT_LIBRARY NAMES lua51 luajit-5.1)

set(BGFX_INSTALL OFF)
set(BGFX_BUILD_EXAMPLES OFF)
set(BGFX_CUSTOM_TARGETS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/bgfx-git)

target_link_libraries(${PROJECT_NAME} yaml-cpp fmt::fmt asio::asio imgui::imgui EnTT::EnTT sol2::sol2 raylib bgfx bx bimg ${LUAJIT_LIBRARY})
target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES SOL_LUAJIT SOL_ALL_SAFETIES_ON)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} winmm)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601 _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN VC_EXTRALEAN)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${LUAJIT_INCLUDE_DIR})

add_compile_options(${PROJECT_NAME} -fsanitize=undefined,address)
add_link_options(${PROJECT_NAME} -fsanitize=undefined,address)
