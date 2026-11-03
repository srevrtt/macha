#include <stdlib.h>

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "include/logger.h"
#include "include/renderer.h"
#include "include/shader.h"
#include "include/rectangle.h"

#include "include/shaders/rect.vert.h"
#include "include/shaders/rect.frag.h"

struct mach_renderer_t {
    mach_shader_t *shader;
    vec4 projection_mat4[4];

    mach_rectangle_t **rect_list;
    int rect_list_size;
    int rect_list_used;
};

mach_renderer_t *mach_renderer_create(int width, int height,
        mach_logger_t *logger) {
    mach_renderer_t *renderer = malloc(sizeof(mach_renderer_t));

    /* set the OpenGL viewport */
    glViewport(0, 0, width, height);

    /* enable the OpenGL features we need */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* now, we can initialize our component lists */
    mach_logger_log(logger, MACH_LOG_INFO,
            "GL Renderer: Initializing CPU data buffers...");

    renderer->rect_list_used = 0;

    /* starting at 20 instead of 1 saves a bunch of realloc calls */
    renderer->rect_list_size = 20;
    renderer->rect_list_used = 0;
    renderer->rect_list = calloc(renderer->rect_list_size,
            sizeof(void*));

    /* make sure the allocation was successful */
    if (renderer->rect_list == NULL) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "Failed to allocate memory for the rectangle component "
                "list");
        mach_logger_set_flag(logger, "MEM_ALLOC_FAILED");
        free(renderer);
        return NULL;
    }

    mach_logger_log(logger, MACH_LOG_INFO,
            "GL Renderer: Creating shaders..");
    renderer->shader = mach_shader_create(logger,
            ___res_shaders_rect_vert,
            ___res_shaders_rect_frag,
            ___res_shaders_rect_vert_len,
            ___res_shaders_rect_frag_len);

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
                "GL Renderer: An OpenGL error has been detected: "
                "GL Error %d",
                error);
        free(renderer->rect_list);
        free(renderer);
        return NULL;
    }

    /* now we can set up the projection matrix */
    glm_ortho(0.0f, (float)width, (float)height,
            0.0f, 0.0f, 1.0f,
            renderer->projection_mat4);

    return renderer;
}

/* adds the rectangle to the renderer; a couple things: first, the renderer
   simply keeps a pointer to the actual rectangle, which means if an update to
   the origin copy occurs, it will automatically update */
void mach_renderer_add_rect(mach_renderer_t *self, mach_logger_t *logger,
        mach_rectangle_t *rect) {
    if (self->rect_list_used >= self->rect_list_size) {
        /* attempt to resize the array if we've run out of space */
        self->rect_list_size *= 2;
        mach_rectangle_t **temp = realloc(self->rect_list,
                self->rect_list_size * sizeof(void*));

        /* if not, then handle the error accordingly */
        if (temp == NULL) {
            mach_logger_log(logger, MACH_LOG_CRITICAL,
                    "Error when attempting to resize the internal "
                    "rectangle array: Memory reallocation failed");
            mach_logger_set_flag(logger, "MEM_ALLOC_FAILED");
            free(self->rect_list);
            return;
        }
    }

    /* add the element */
    self->rect_list[self->rect_list_used++] = rect;
}

void mach_renderer_draw(mach_renderer_t *self) {
    /* first, clear the screen */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* then, we can use the shader program and assign our projection matrix */
    mach_shader_use(self->shader);
    mach_shader_mat4(self->shader, "projection",
            self->projection_mat4);

    /* ...and draw the buffers for each rectangle */
    for (int i = 0; i < self->rect_list_used; i++) {
        mach_rectangle_draw(self->rect_list[i], self->shader);
    }

    /* at this point, we've finished drawing on the back buffer, so it's now up
       to the application to swap the buffers after this function call. */
}

void mach_renderer_destroy(mach_renderer_t *self) {
    if (self) {
        mach_shader_destroy(self->shader);

        /* destroy all objects */
        for (int i = 0; i < self->rect_list_used; i++) {
            mach_rectangle_destroy(self->rect_list[i]);
        }

        free(self->rect_list);
        free(self);
    }
}

