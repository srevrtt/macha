#ifndef FONT_LIB_H
#define FONT_LIB_H

typedef struct mach_font_lib_t mach_font_lib_t;
typedef struct mach_logger_t mach_logger_t;
typedef struct FT_LibraryRec_* FT_Library;

mach_font_lib_t *mach_font_lib_create(mach_logger_t *logger);
FT_Library mach_font_lib_ft(mach_font_lib_t *self);
void mach_font_lib_destroy(mach_font_lib_t *self);

#endif

