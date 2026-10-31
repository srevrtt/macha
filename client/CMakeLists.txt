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

    # Disable OpenGL deprecation for Apple devices
    add_definitions(-Wno-deprecated-declarations)

    set(APPLE_FRAMEWORKS "-framework Foundation -framework Cocoa")
endif()

# Enable address and leak sanitizers if we're doing a Debug build
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(SAN_FLAGS
        -fsanitize=address
        -fno-omit-frame-pointer
        -g
    )

    add_compile_options(${SAN_FLAGS})
    add_link_options(${SAN_FLAGS})
endif()

set(SRCS
    src/app.c
    src/logger.c
	src/window.c
	src/main.c
	${EXTERNAL_SRCS}
	${PLATFORM_SRCS}
)

add_executable(macha_client ${SRCS})

# Convert the GLSL shader source code files to C headers
add_custom_command(
    TARGET macha_client
    PRE_BUILD
    COMMAND echo "Converting GLSL shaders to C headers for compilation..."
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/convert_shaders.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

target_include_directories(macha_client PRIVATE external/)
target_link_libraries(macha_client PRIVATE PkgConfig::cglm ${APPLE_FRAMEWORKS})

