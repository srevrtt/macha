cmake_minimum_required(VERSION 4.0)
project(macha_client VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

cmake_pkg_config(IMPORT cglm REQUIRED)

set(EXTERNAL_SRCS
	external/glad/glad.c
)

# Create the shaders directory
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/include/shaders")

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
        -fsanitize=undefined
        -fno-omit-frame-pointer
        -g
    )

    add_compile_options(${SAN_FLAGS})
    add_link_options(${SAN_FLAGS})
endif()

set(SRCS
    src/renderer.c
    src/rectangle.c
    src/shader.c
    src/app.c
    src/logger.c
	src/window.c
	src/main.c
	${EXTERNAL_SRCS}
	${PLATFORM_SRCS}
)

# Relative to ./res/shaders/
set(SHADER_SRCS
    rect.vert
    rect.frag
)

add_executable(macha_client ${SRCS})

# Convert the GLSL shader source code files to C headers
set(SHADER_OUTPUTS "")
foreach(file_path IN LISTS SHADER_SRCS)
    set(OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/include/shaders/${file_path}.h")
    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/convert_shader.sh "${file_path}" "${OUTPUT_FILE}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/${file_path}"
        VERBATIM
    )
    list(APPEND SHADER_OUTPUTS "${OUTPUT_FILE}")
endforeach()

add_custom_target(run_shader_conversion
    ALL
    DEPENDS ${SHADER_OUTPUTS}
)

add_dependencies(macha_client run_shader_conversion)

target_include_directories(macha_client PRIVATE external/)
target_link_libraries(macha_client PRIVATE PkgConfig::cglm ${APPLE_FRAMEWORKS})

