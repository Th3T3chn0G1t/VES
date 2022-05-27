cmake_minimum_required(VERSION 3.18.1)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0111 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

include(vcpkg.cmake)

project(VES)

file(GLOB_RECURSE SRC_FILES vendor/*.cpp vendor/*.hpp vendor/*.c vendor/*.h src/*.cpp src/*.hpp src/*.c src/*.h)
add_executable(${PROJECT_NAME} ${SRC_FILES})

set(raylib_USE_STATIC_LIBS ON)
find_package(raylib CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_path(LUAJIT_INCLUDE_DIR luajit.h PATH_SUFFIXES luajit luajit-2.0 imgui::imgui)
find_library(LUAJIT_LIBRARY NAMES lua51 luajit-5.1)

target_link_libraries(${PROJECT_NAME} fmt::fmt asio::asio EnTT::EnTT sol2::sol2 raylib ${LUAJIT_LIBRARY})
target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} winmm)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601 _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN VC_EXTRALEAN)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)