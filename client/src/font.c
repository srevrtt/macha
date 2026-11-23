#include <stdlib.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <glad/glad.h>

#include "include/logger.h"
#include "include/renderer.h"
#include "include/font.h"
#include "include/font_lib.h"
#include "include/utils.h"

#include "include/shader.h"
#include "include/shaders/text.vert.h"
#include "include/shaders/text.frag.h"

#include <cglm/cglm.h>

struct mach_font_t {
    FT_Face face;
    char *char_map;
    size_t char_map_len;
    unsigned int *advances;
    int *offsets;
    ivec2 *sizes;
    ivec2 *bearings;
    float **vertex_data;

    mach_shader_t *shader;
    mat4 projection;

    unsigned int atlas_tex;
    unsigned char *atlas_buf;
    unsigned int *atlas_indices;
    ivec2 atlas_size;

    unsigned int vao;
};

/* creates and loads a TTF font */
mach_font_t *mach_font_create(mach_font_lib_t *lib, mach_logger_t *logger,
        mach_renderer_t *renderer, const char *font_path, int font_size) {
    FT_Face face;
    if (FT_New_Face(mach_font_lib_ft(lib),
                font_path, 0, &face)) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "Failed to load font \"%s\"", font_path);
        return NULL;
    }

    /* set the font size */
    FT_Set_Pixel_Sizes(face, 0, font_size);

    /* create the font object */
    mach_font_t *font = malloc(sizeof(mach_font_t));
    font->face = face;
    font->char_map_len = 0;

    size_t char_map_size = 95;
    font->char_map = malloc(char_map_size);
    font->advances = malloc(char_map_size * sizeof(unsigned int));
    font->offsets = malloc(char_map_size * sizeof(int));
    font->sizes = malloc(char_map_size * sizeof(ivec2));
    font->bearings = malloc(char_map_size * sizeof(ivec2));
    font->vertex_data = malloc(char_map_size * sizeof(void*));

    /* start with 2KiB */
    size_t atlas_buf_mem_size = 2048;
    size_t atlas_buf_mem_used = 0;
    font->atlas_buf = malloc(atlas_buf_mem_size);

    /* create and copy the indices (they're the same for every character) */
    unsigned int atlas_indices[] = {
        0, 1, 3,
        2, 1, 3
    };

    font->atlas_indices = malloc(sizeof(atlas_indices));
    memcpy(font->atlas_indices, atlas_indices,
            sizeof(atlas_indices));

    int atlas_width = 0;
    int atlas_height = 0;

    /* first, let's make a pass to determine the width and height of the
       atlas */
    for (char c = 32; c < 127; c++) {
        // TODO: Use FT_Load_Glyph instead
        if (FT_Load_Char(face, c,
                    FT_LOAD_RENDER)) {
            mach_logger_log(logger, MACH_LOG_WARN,
                    "Failed to load character %c at font path %s.", c,
                    font_path);
            continue;
        }

        atlas_width += face->glyph->bitmap.width;
        atlas_height = (face->glyph->bitmap.rows > atlas_height) ?
            face->glyph->bitmap.rows : atlas_height;
    }

    /* now, we can create the texture atlas with a default en-us charset */
    glGenTextures(1, &font->atlas_tex);
    glBindTexture(GL_TEXTURE_2D, font->atlas_tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
            atlas_width, atlas_height, 0,
            GL_RED, GL_UNSIGNED_BYTE, NULL);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* texture options */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);

    bool failed_alloc = false;
    int current_x = 0;

    for (char c = 32; c < 127; c++) {
        // TODO: Use FT_Load_Glyph instead
        if (FT_Load_Char(face, c,
                    FT_LOAD_RENDER)) {
            mach_logger_log(logger, MACH_LOG_WARN,
                    "Failed to load character %c at font path %s.", c,
                    font_path);
            continue;
        }

        /* append the data */
        int idx = c - 32;

        glTexSubImage2D(GL_TEXTURE_2D, 0, current_x,
                0, face->glyph->bitmap.width,
                face->glyph->bitmap.rows, GL_RED,
                GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        font->char_map[idx] = c;
        font->advances[idx] = (int)(face->glyph->advance.x) >> 6;
        font->offsets[idx] = current_x;

        /* font size */
        ivec2 size = {
            face->glyph->bitmap.width, face->glyph->bitmap.rows
        };
        glm_ivec2_copy(size, font->sizes[idx]);

        /* font bearing */
        ivec2 bearing = {
            face->glyph->bitmap_left, face->glyph->bitmap_top
        };
        glm_ivec2_copy(bearing, font->bearings[idx]);

        font->char_map_len++;

        /* vertex data */
        float u_min = (float)current_x / atlas_width;
        float u_max = (float)(current_x + face->glyph->bitmap.width)
            / atlas_width;
        float v_min = 0.0f;
        float v_max = (float)face->glyph->bitmap.rows / atlas_height;

        float vertex_data[] = {
            0.5f, -0.5f,        u_max, v_min,
            0.5f, 0.5f,         u_max, v_max,
            -0.5f, 0.5f,        u_min, v_max,
            -0.5f, -0.5f,       u_min, v_min
        };

        font->vertex_data[idx] = malloc(sizeof(vertex_data));
        memcpy(font->vertex_data[idx], vertex_data,
                sizeof(vertex_data));

        current_x += face->glyph->bitmap.width;
    }

    /* check to see if an allocation failed */
    if (failed_alloc) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "Memory reallocation of font character map failed");
        return NULL;
    }

    ivec2 atlas_size = { atlas_width, atlas_height };
    glm_ivec2_copy(atlas_size, font->atlas_size);

    /* initialize the OpenGL objects we need */
    glGenVertexArrays(1, &font->vao);
    glBindVertexArray(font->vao);

    /* create the shader */
    font->shader = mach_shader_create(logger,
            ___res_shaders_text_vert,
            ___res_shaders_text_frag,
            ___res_shaders_text_vert_len,
            ___res_shaders_text_frag_len);

    // TODO: Somehow fix alignment
    glm_mat4_copy(mach_renderer_get_projection(renderer),
            font->projection);

    return font;
}

void mach_font_draw_text(mach_font_t *self, int x, int y, unsigned int color,
        const char *text) {
    mach_shader_use(self->shader);
    glBindVertexArray(self->vao);
    glBindTexture(GL_TEXTURE_2D, self->atlas_tex);

    /* upload the text color */
    vec3 text_color;
    mach_color_from_hex(color, text_color);
    mach_shader_vec3(self->shader, "f_textColor", text_color);

    /* upload index buffer data */
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            6 * sizeof(unsigned int), self->atlas_indices,
            GL_STATIC_DRAW);

    /* upload the model matrix */
    mach_shader_mat4(self->shader, "projection",
            self->projection);

    int current_x = x;

    for (int chr = 0; chr < strlen(text); chr++) {
        int i = text[chr] - 32;

        if (i >= 0) {
            /* create the model matrix */
            mat4 model;
            glm_mat4_identity(model);

            int width = self->sizes[i][0];
            int height = self->sizes[i][1];
            int xpos = current_x + self->bearings[i][0];
            int ypos = y - self->bearings[i][1];

            vec3 translation = { xpos + width * 0.5f, ypos + height * 0.5f, 0.0f };
            vec3 scale = { width, height, 0.0f };
            glm_translate(model, translation);
            glm_scale(model, scale);

            /* upload the model matrix */
            mach_shader_mat4(self->shader, "model", model);

            /* upload the already defined vertex data for our character */
            unsigned int vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
                    self->vertex_data[i], GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT,
                    GL_FALSE, 4 * sizeof(float),
                    (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT,
                    GL_FALSE, 4 * sizeof(float),
                    (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            /* now we can draw! */
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                    0);

            /* advance the X pointer */
            current_x += self->advances[i];
            glDeleteBuffers(1, &vbo);
        }
    }

    glDeleteBuffers(1, &ebo);
}

void mach_font_destroy(mach_font_t *self) {
    if (self) {
        FT_Done_Face(self->face);
        free(self);
    }
}

