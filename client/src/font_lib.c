#include <stdlib.h>

#include <freetype/freetype.h>

#include "include/logger.h"
#include "include/font_lib.h"

struct mach_font_lib_t {
    FT_Library ft;
};

mach_font_lib_t *mach_font_lib_create(mach_logger_t *logger) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        mach_logger_log(logger, MACH_LOG_CRITICAL,
                "Failed to initialize the FreeType library");
        return NULL;
    }

    mach_font_lib_t *lib = malloc(sizeof(mach_font_lib_t));
    lib->ft = ft;

    return lib;
}

FT_Library mach_font_lib_ft(mach_font_lib_t *self) {
    return self->ft;
}

void mach_font_lib_destroy(mach_font_lib_t *self) {
    if (self) {
        FT_Done_FreeType(self->ft);
        free(self);
    }
}

