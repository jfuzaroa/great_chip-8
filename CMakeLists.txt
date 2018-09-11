cmake_minimum_required(VERSION 3.12)
project(great_chip-8)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(great_chip-8 ${SOURCES})
