#ifndef FONT_H
#define FONT_H

#include "cglm/types.h"

typedef struct mach_font_t mach_font_t;
typedef struct mach_font_lib_t mach_font_lib_t;
typedef struct mach_logger_t mach_logger_t;
typedef struct mach_renderer_t mach_renderer_t;
typedef struct mach_shader_t mach_shader_t;

typedef struct {
    vec2 size;
    vec2 bearing;
    float *vertex_data;
    unsigned int *index_data;
    unsigned int advance;
} mach_char_info_t;

mach_font_t *mach_font_create(mach_font_lib_t *lib, mach_logger_t *logger,
		mach_renderer_t *renderer, const char *font_path, int font_size);
mach_char_info_t mach_font_get_char(mach_font_t *self, char c);
mach_shader_t *mach_font_get_shader(mach_font_t *self);
unsigned int mach_font_get_vao(mach_font_t *self);
unsigned int mach_font_get_atlas(mach_font_t *self);
void mach_font_destroy(mach_font_t *self);

#endif

