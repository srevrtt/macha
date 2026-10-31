#include <stdlib.h>

#include <glad/glad.h>

#include "include/rectangle.h"

struct mach_rectangle_t {
    unsigned int vertex_buffer;
    unsigned int element_buffer;
    unsigned int vertex_array;
};

mach_rectangle_t *mach_rectangle_create() {
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
        2, 3, 1
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

    return rect;
}

void mach_rectangle_draw(mach_rectangle_t *self) {
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

