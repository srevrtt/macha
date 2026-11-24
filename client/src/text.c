#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>

#include "include/text.h"
#include "include/font.h"
#include "include/shader.h"
#include "include/utils.h"

#include <cglm/cglm.h>

struct mach_text_t {
    mach_font_t *font;
    char *text;
    vec2 pos;

    mach_char_info_t *info;
    size_t prev_info_size;

    unsigned int atlas;
    unsigned int vao;
    mach_shader_t *shader;
};

/* helper function */
static void mach_text_refresh(mach_text_t *self) {
    int len = strlen(self->text);

    /* initialize the character information struct array */
    if (self->prev_info_size == 0) {
        self->info = malloc(len * sizeof(mach_char_info_t));
    } else {
        /* grow or shrink the array size */
        void *info = realloc(self->info, len *
                sizeof(mach_char_info_t));

        if (!info) return;
        self->info = info;
    }
    self->prev_info_size = len;

    /* now go through and load each char */
    glBindVertexArray(self->vao);
    for (int i = 0; i < len; i++) {
        char c = self->text[i];
        self->info[i] = mach_font_get_char(self->font, c);
    }
}

mach_text_t *mach_text_create(mach_font_t *font, int xpos, int ypos,
        const char *text) {
    mach_text_t *self = malloc(sizeof(mach_text_t));

    self->font = font;
    self->text = malloc(strlen(text) + 1);
    strcpy(self->text, text);

    self->shader = mach_font_get_shader(font);
    self->atlas = mach_font_get_atlas(font);
    self->vao = mach_font_get_vao(font);

    vec2 pos = { xpos, ypos };
    glm_vec2_copy(pos, self->pos);

    self->prev_info_size = 0;
    mach_text_refresh(self);

    return self;
}

void mach_text_change(mach_text_t *self, const char *new_text) {
    void *tmp = realloc(self->text, strlen(new_text) + 1);
    if (!tmp) return;

    self->text = tmp;
    strcpy(self->text, new_text);
    mach_text_refresh(self);
}

void mach_text_render(mach_text_t *self, unsigned int color, mat4 projection) {
    mach_shader_use(self->shader);
    glBindVertexArray(self->vao);
    glBindTexture(GL_TEXTURE_2D, self->atlas);

    /* upload the text color to the shader */
    vec3 text_color;
    mach_color_from_hex(color, text_color);
    mach_shader_vec3(self->shader, "f_textColor", text_color);

    /* upload the projection matrix */
    mach_shader_mat4(self->shader, "projection", projection);

    int current_x = self->pos[0];
    mat4 model;

    // TODO: Optimize index_data to be global rather than per char
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 *
            sizeof(unsigned int), self->info[0].index_data,
            GL_STATIC_DRAW);

    for (int i = 0; i < strlen(self->text); i++) {
        /* create the model matrix */
        glm_mat4_identity(model);

        int width = self->info[i].size[0];
        int height = self->info[i].size[1];
        int xpos = current_x + self->info[i].bearing[0];
        int ypos = self->pos[1] - self->info[i].bearing[1];

        vec3 translate = { xpos + width * 0.5f, ypos + height * 0.5f,
            0.0f };
        vec3 scale = { width, height, 0.0f };
        glm_translate(model, translate);
        glm_scale(model, scale);

        /* upload the model matrix */
        mach_shader_mat4(self->shader, "model", model);

        /* next, load the GL buffer data for the character */
        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
                self->info[i].vertex_data, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT,
                GL_FALSE, 4 * sizeof(float),
                (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT,
                GL_FALSE, 4 * sizeof(float),
                (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        /* draw and advance */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                0);
        current_x += self->info[i].advance;


        /* clean up */
        glDeleteBuffers(1, &vbo);
    }

    glDeleteBuffers(1, &ebo);
}


void mach_text_destroy(mach_text_t *self) {
    if (self) {
        free(self);
    }
}

