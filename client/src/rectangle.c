#include <stdlib.h>

#include <glad/glad.h>
#include <cglm/cglm.h>

#include "include/shader.h"
#include "include/rectangle.h"

struct mach_rectangle_t {
    unsigned int vertex_buffer;
    unsigned int element_buffer;
    unsigned int vertex_array;

    vec4 model[4];
    float x;
    float y;
    float w;
    float h;
};

mach_rectangle_t *mach_rectangle_create(int x, int y, int w, int h) {
    mach_rectangle_t *rect = malloc(sizeof(mach_rectangle_t));

    /* start shipping the following buffer data with a VAO */
    glGenVertexArrays(1, &rect->vertex_array);
    glBindVertexArray(rect->vertex_array);

    /* define the vertex and index data on the CPU */
    float vertices[] = {
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        2, 1, 3
    };

    /* now we can upload this data to the GPU, starting with the VBO */
    glGenBuffers(1, &rect->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rect->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
          vertices, GL_STATIC_DRAW);

    /* next, do the same thing for the EBO */
    glGenBuffers(1, &rect->element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
            rect->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
            indices, GL_STATIC_DRAW);

    /* finally, actually tell OpenGL about how to use the VAO */
    glVertexAttribPointer(0, 3, GL_FLOAT,
            GL_FALSE, 3 * sizeof(float),
            (void*)0);
    glEnableVertexAttribArray(0);

    /* now, we can initialize the model matrix & configure it to the size that
       was requested (this does happen again every time we update as well) */
    mach_rectangle_transform(rect, x, y, w, h);

    return rect;
}

/* performs a transformation operation on the rectangle; this function does
   ignore parameters that are "-1"; DO NOT SET ANY ARGUMENT TO NULL, as this'll
   make the function treat it as 0 instead of -1, so for any property you do
   *not* want to change, simply pass -1 in place */
void mach_rectangle_transform(mach_rectangle_t *self, int x, int y, int w,
        int h) {
    /* set the properties */
    if (x != -1) self->x = (float)x;
    if (y != -1) self->y = (float)y;
    if (w != -1) self->w = (float)w;
    if (h != -1) self->h = (float)h;

    glm_mat4_identity(self->model);

    /* note: the reason why we're adding an offset is because, for some reason
       (still not sure?), whenever an OpenGL context is created, each object is
       center-aligned, which is not really what we want in this case, so that
       means that this is just a workaround :D */
    vec3 translation = {
        self->x + self->w * 0.5f, self->y + self->h * 0.5f, 0.0f
    };
    glm_translate(self->model, translation);

    vec3 scale = {
        self->w, self->h, 0.0f
    };
    glm_scale(self->model, scale);
}

/* draws the rectangle to the renderer; make sure the shader has been enabled/
   "used" before calling this */
void mach_rectangle_draw(mach_rectangle_t *self, mach_shader_t *shader) {
    mach_shader_mat4(shader, "model", self->model);

    glBindVertexArray(self->vertex_array);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
            NULL);
}

void mach_rectangle_destroy(mach_rectangle_t *self) {
    if (self) {
        glDeleteBuffers(1, &self->element_buffer);
        glDeleteBuffers(1, &self->vertex_buffer);
        glDeleteVertexArrays(1, &self->vertex_array);
        self->element_buffer = 0;
        self->vertex_buffer = 0;
        self->vertex_array = 0;

        free(self);
    }
}

