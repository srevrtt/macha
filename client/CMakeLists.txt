cmake_minimum_required(VERSION 4.0)
project(macha_client VERSION 0.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

cmake_pkg_config(IMPORT cglm REQUIRED)

set(EXTERNAL_SRCS
	external/glad/glad.c
)

set(SRCS ${EXTERNAL_SRCS}
	src/main.c
)

add_executable(macha_client ${SRCS})

target_include_directories(macha_client PRIVATE external/)
target_link_libraries(macha_client PRIVATE PkgConfig::cglm)

