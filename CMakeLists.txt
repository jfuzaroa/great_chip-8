cmake_minimum_required(VERSION 3.12)
project(great_chip-8)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(glfw3 3.2 REQUIRED)

include_directories(include)
include_directories(${OPENGL_INCLUDE_DIR})

file(GLOB SOURCES "src/*.c")

add_executable(great_chip-8 ${SOURCES})
