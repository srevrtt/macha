cmake_minimum_required(VERSION 4.0)
project(macha_client VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

cmake_pkg_config(IMPORT cglm REQUIRED)

set(EXTERNAL_SRCS
	external/glad/glad.c
)

# Set platform-specific source files
if(APPLE)
	set(PLATFORM_SRCS
		src/mac_window.m
	)

	# TODO: ???
	set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -framework Foundation -framework Cocoa")
endif()

set(SRCS
	${EXTERNAL_SRCS}
	${PLATFORM_SRCS}
    src/app.c
    src/logger.c
	src/window.c
	src/main.c
)

add_executable(macha_client ${SRCS})

target_include_directories(macha_client PRIVATE external/)
target_link_libraries(macha_client PRIVATE PkgConfig::cglm)

