cmake_minimum_required(VERSION 4.0)

include(cmake/pico_sdk_import.cmake)

project(picodb C CXX ASM)

set(SRCS src/main.c)

pico_sdk_init()

add_executable(picodb ${SRCS})

target_link_libraries(picodb pico_stdlib)
target_compile_features(picodb PRIVATE c_std_11)

pico_add_extra_outputs(picodb)

