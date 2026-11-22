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

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* set the font size */
    FT_Set_Pixel_Sizes(face, 0, font_size);


    /* create the font object */
    mach_font_t *font = malloc(sizeof(mach_font_t));
    font->face = face;
    font->char_map_len = 0;

    size_t char_map_size = 94;
    font->char_map = malloc(char_map_size);
    font->advances = malloc(char_map_size * sizeof(unsigned int));
    font->offsets = malloc(char_map_size * sizeof(int));
    font->sizes = malloc(char_map_size * sizeof(ivec2));
    font->bearings = malloc(char_map_size * sizeof(ivec2));
    font->vertex_data = malloc(char_map_size * 16 * sizeof(float));

    /* start with 1KiB */
    size_t atlas_buf_mem_size = 1024;
    size_t atlas_buf_mem_used = 0;
    font->atlas_buf = malloc(atlas_buf_mem_size);

    /* create and copy the indices (they're the same for every character) */
    unsigned int atlas_indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    font->atlas_indices = malloc(sizeof(atlas_indices));
    memcpy(font->atlas_indices, atlas_indices,
            sizeof(atlas_indices));

    int current_width = 0;

    /* now, we can create the texture atlas with a default en-us charset */
    bool failed_alloc = false;
    for (char c = 33; c < 127; c++) {
        // TODO: Use FT_Load_Glyph instead
        if (FT_Load_Char(font->face, c,
                    FT_LOAD_RENDER)) {
            mach_logger_log(logger, MACH_LOG_WARN,
                    "Failed to load character %c at font path %s.", c,
                    font_path);
            continue;
        }

        /* append the data */
        int idx = c - 33;

        font->char_map[idx] = c;
        font->advances[idx] = face->glyph->advance.x;
        font->offsets[idx] = current_width;

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

        /* resize buffer if needed */
        if (atlas_buf_mem_used >= atlas_buf_mem_size) {
            atlas_buf_mem_size *= 2;
            void *tmp = realloc(font->atlas_buf,
                    atlas_buf_mem_size);

            if (!tmp) {
                failed_alloc = true;
                break;
            }
            font->atlas_buf = tmp;
        }

        /* now we can append the buffer data */
        memcpy(font->atlas_buf + sizeof(atlas_buf_mem_used),
                face->glyph->bitmap.buffer,
                face->glyph->bitmap.pitch * face->glyph->bitmap.rows);
        current_width += face->glyph->bitmap.width;
        atlas_buf_mem_used += face->glyph->bitmap.pitch * face->glyph->bitmap.rows;
    }

    /* check to see if an allocation failed */
    if (failed_alloc) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "Memory reallocation of font character map failed");
        return NULL;
    }

    float map_width = current_width;
    ivec2 atlas_size = { map_width, font_size };
    glm_ivec2_copy(atlas_size, font->atlas_size);

    /* next up, let's upload the UV (and vertex) coordinates, now that we know
       the total width of the atlas */
    for (int i = 0; i < font->char_map_len; i++) {
        float offset = font->offsets[i];
        float width = font->sizes[i][0];
        float height = font->sizes[i][1];
        float x_bearing = font->bearings[i][0];
        float y_bearing = font->bearings[i][1];

        float right_uv = (offset + width) / current_width;
        float left_uv = offset / current_width;

        float vertex_data[] = {
            0.5f, -0.5f,        right_uv, 0.0f,
            0.5f, 0.5f,         right_uv, 1.0f,
            -0.5f, 0.5f,        left_uv, 1.0f,
            -0.5f, -0.5f,       left_uv, 0.0f
        };

        font->vertex_data[i] = malloc(sizeof(vertex_data));
        memcpy(font->vertex_data[i], vertex_data,
                sizeof(vertex_data));
    }

    /* initialize the OpenGL objects we need */
    glGenVertexArrays(1, &font->vao);
    glBindVertexArray(font->vao);

    /* let's finally finish it up and create the texture! */
    glGenTextures(1, &font->atlas_tex);
    glBindTexture(GL_TEXTURE_2D, font->atlas_tex);

    /* texture options */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);

    /* generate the texture */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
            atlas_size[0], atlas_size[1], 0,
            GL_RED, GL_UNSIGNED_BYTE, font->atlas_buf);

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
    glBindTexture(GL_TEXTURE_2D, self->atlas_tex);\

    /* upload the text color */
    vec3 text_color;
    mach_color_from_hex(color, text_color);
    mach_shader_vec3(self->shader, "f_textColor", text_color);

    /* create the model matrix */
    mat4 model;
    glm_mat4_identity(model);
    vec3 translation = { x, y, 0.0f };
    glm_translate(model, translation);

    /* upload the model and projection matrices */
    mach_shader_mat4(self->shader, "model", model);
    mach_shader_mat4(self->shader, "projection",
            self->projection);

    /* upload the already defined vertex data for our character */
    char c = 'X';
    int i = c - 33;

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
            self->vertex_data[i], GL_STATIC_DRAW);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            6 * sizeof(unsigned int), self->atlas_indices,
            GL_STATIC_DRAW);

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
}

void mach_font_destroy(mach_font_t *self) {
    if (self) {
        FT_Done_Face(self->face);
        free(self);
    }
}

