#include <stdlib.h>
#include <stdbool.h>

#include <glad/glad.h>

#include "include/logger.h"
#include "include/shader.h"

struct mach_shader_t {
    unsigned int shader;
};

/* helper function to check shader compilation status & log if needed */
static bool shader_success(mach_logger_t *logger, unsigned int shader) {
    /* check status */
    int status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    /* shader compiled succesfully */
    if (status == GL_TRUE) {
        return true;
    }

    /* get the info log buffer length */
    int log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

    /* get the actual info log */
    char *info_log = malloc(log_length * sizeof(char));
    glGetShaderInfoLog(shader, log_length, NULL,
            info_log);

    /* now we can log it */
    mach_logger_log(logger, MACH_LOG_CRITICAL,
            "Failed to compile shader: %s", info_log);
    mach_logger_set_flag(logger, "GL_SHADER_COMP_FAILED");

    free(info_log);
    return false;
}

/* helper function to check program compilation status */
static bool program_success(mach_logger_t *logger, unsigned int program) {
    /* check status */
    int status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_TRUE) {
        return true;
    }

    /* if the linking process was unsuccessful, we need to figure out why */
    int log_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

    char *info_log = malloc(log_length * sizeof(char));
    glGetProgramInfoLog(program, log_length, NULL,
            info_log);

    mach_logger_log(logger, MACH_LOG_CRITICAL,
            "Failed to link shader program: %s", info_log);
    mach_logger_set_flag(logger, "GL_SHADER_LINK_FAILED");

    free(info_log);
    return false;
}

/* compiles the vertex and fragment shaders into a shader program */
mach_shader_t *mach_shader_create(mach_logger_t *logger,
        unsigned char *vertex_src, unsigned char *fragment_src,
        unsigned int vertex_len, unsigned int fragment_len) {
    mach_shader_t *shader = malloc(sizeof(mach_shader_t));

    /* create and compile the shaders */

    /* vertex shader */
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1,
            (const GLchar *const *)&vertex_src,
            (const GLint *)&vertex_len);
    glCompileShader(vertex_shader);

    if (!shader_success(logger, vertex_shader)) {
        return NULL;
    }

    /* fragment shader */
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1,
            (const GLchar *const *)&fragment_src,
            (const GLint*)&fragment_len);
    glCompileShader(fragment_shader);

    if (!shader_success(logger, fragment_shader)) {
        return NULL;
    }

    /* shader program */
    shader->shader = glCreateProgram();
    glAttachShader(shader->shader, vertex_shader);
    glAttachShader(shader->shader, fragment_shader);
    glLinkProgram(shader->shader);

    /* shader program error handling */
    if (!program_success(logger, shader->shader)) {
        return NULL;
    }

    /* if everything was successful, free any memory that has been used up that
       we don't need */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader;
}

/* simply calls `glUseProgram` on the internal shader program */
void mach_shader_use(mach_shader_t *self) {
    glUseProgram(self->shader);
}

void mach_shader_destroy(mach_shader_t *self) {
    if (self) {
        glDeleteProgram(self->shader);
        self->shader = 0;
        free(self);
    }
}

