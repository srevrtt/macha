#include <stdlib.h>

#include <ft2build.h>
#include <freetype/freetype.h>

#include "include/logger.h"
#include "include/font.h"
#include "include/font_lib.h"

struct mach_font_t {
    FT_Face face;
};

// TODO: separate `mach_font_lib_t` struct for FT_Library

/* creates and loads a TTF font */
mach_font_t *mach_font_create(mach_font_lib_t *lib, mach_logger_t *logger,
        const char *font_path, int font_size) {
    FT_Face face;
    if (FT_New_Face(mach_font_lib_ft(lib),
                font_path, 0, &face)) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "Could not load font \"%s\"", font_path);
        return NULL;
    }

    /* set the font size */
    FT_Set_Pixel_Sizes(face, 0, font_size);

    /* create the font object */
    mach_font_t *font = malloc(sizeof(mach_font_t));
    font->face = face;

    return font;
}

void mach_font_destroy(mach_font_t *self) {
    if (self) {
        FT_Done_Face(self->face);
        free(self);
    }
}

