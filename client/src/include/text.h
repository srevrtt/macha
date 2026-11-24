#ifndef TEXT_H
#define TEXT_H

#include <cglm/types.h>

typedef struct mach_text_t mach_text_t;
typedef struct mach_font_t mach_font_t;

mach_text_t *mach_text_create(mach_font_t *font, int xpos, int ypos,
        const char *text);
void mach_text_change(mach_text_t *self, const char *new_text);
void mach_text_render(mach_text_t *self, unsigned int color, mat4 projection);
void mach_text_destroy(mach_text_t *self);

#endif
