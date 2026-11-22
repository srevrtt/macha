#include <stdlib.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <cglm/cglm.h>
#include <glad/glad.h>

#include "include/logger.h"
#include "include/font.h"
#include "include/font_lib.h"

struct mach_font_t {
    FT_Face face;
    char *char_map;
    size_t char_map_len;
    unsigned int *advances;
    ivec2 *sizes;
    ivec2 *bearings;

    unsigned int atlas_tex;
    unsigned char *atlas_buf;
    ivec2 atlas_size;
};

/* creates and loads a TTF font */
mach_font_t *mach_font_create(mach_font_lib_t *lib, mach_logger_t *logger,
        const char *font_path, int font_size) {
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

    size_t char_map_size = 94;
    font->char_map = malloc(char_map_size);
    font->advances = malloc(char_map_size * sizeof(unsigned int));
    font->sizes = malloc(char_map_size * sizeof(ivec2));
    font->bearings = malloc(char_map_size * sizeof(ivec2));

    /* start with 1KiB */
    size_t atlas_buf_mem_size = 1024;
    size_t atlas_buf_mem_used = 0;
    font->atlas_buf = malloc(atlas_buf_mem_size);

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

        /* resize the character map data if needed */
        if (font->char_map_len >= char_map_size) {
            char_map_size *= 2;

            /* char_map */
            void *tmp = realloc(font->char_map, char_map_size);
            if (!tmp) {
                failed_alloc = true;
                break;
            }
            font->char_map = tmp;

            /* advances */
            tmp = realloc(font->advances, char_map_size *
                    sizeof(unsigned int));
            if (!tmp) {
                failed_alloc = true;
                break;
            }
            font->advances = tmp;

            /* sizes */
            tmp = realloc(font->sizes, char_map_size *
                    sizeof(ivec2));
            if (!tmp) {
                failed_alloc = true;
                break;
            }
            font->sizes = tmp;

            /* bearings */
            tmp = realloc(font->bearings, char_map_size *
                    sizeof(ivec2));
            if (!tmp) {
                failed_alloc = true;
                break;
            }
            font->bearings = tmp;
        }

        /* append the data */
        int idx = c - 33;

        font->char_map[idx] = c;
        font->advances[idx] = font->face->glyph->advance.x;

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
    }

    /* check to see if an allocation failed */
    if (failed_alloc) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "Memory reallocation of font character map failed");
        return NULL;
    }

    /* let's finally finish it up and create the texture! */
    ivec2 atlas_size = { current_width, font_size };
    glm_ivec2_copy(atlas_size, font->atlas_size);

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

    return font;
}

void mach_font_destroy(mach_font_t *self) {
    if (self) {
        FT_Done_Face(self->face);
        free(self);
    }
}

