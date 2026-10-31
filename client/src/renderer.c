#include <stdlib.h>

#include <glad/glad.h>

#include "include/logger.h"
#include "include/renderer.h"
#include "include/shader.h"
#include "include/rectangle.h"

#include "include/shaders/square.vert.h"
#include "include/shaders/square.frag.h"

struct mach_renderer_t {
    mach_shader_t *shader;
    mach_rectangle_t *rect;
};

mach_renderer_t *mach_renderer_create(int width, int height,
        mach_logger_t *logger) {
    mach_renderer_t *renderer = malloc(sizeof(mach_renderer_t));

    /* set the OpenGL viewport */
    glViewport(0, 0, width, height);

    /* now actually create the objects needed to render */
    mach_logger_log(logger, MACH_LOG_INFO,
            "GL Renderer: Creating buffers..");
    renderer->rect = mach_rectangle_create();

    mach_logger_log(logger, MACH_LOG_INFO,
            "GL Renderer: Creating shaders..");
    renderer->shader = mach_shader_create(logger,
            ___res_shaders_square_vert,
            ___res_shaders_square_frag,
            ___res_shaders_square_vert_len,
            ___res_shaders_square_frag_len);

    /* handle errors if the shader creation failed */
    if (renderer->shader == NULL) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "GL Renderer: Shader creation failed with flag %s",
                mach_logger_get_flag(logger, NULL));
        return NULL;
    }

    /* check for OpenGL errors */
    int error = glGetError();

    if (error != GL_NO_ERROR) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "GL Renderer: An OpenGL error has been detected: GL Error %d",
                error);
        return NULL;
    }

    return renderer;
}

void mach_renderer_draw(mach_renderer_t *self) {
    /* first, clear the screen */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* then, we can use the shader program and draw the buffers for each
       rectangle */
    mach_shader_use(self->shader);
    mach_rectangle_draw(self->rect);

    /* at this point, we've finished drawing on the back buffer, so it's now up
       to the application to swap the buffers after this function call. */
}

void mach_renderer_destroy(mach_renderer_t *self) {
    if (self) {
        mach_shader_destroy(self->shader);
        mach_rectangle_destroy(self->rect);
        free(self);
    }
}

