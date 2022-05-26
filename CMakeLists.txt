cmake_minimum_required(VERSION 3.21)

project(VES)

file(GLOB SOURCE ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/*.cpp)

add_executable(VES exec ${SOURCE})