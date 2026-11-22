#ifndef FONT_H
#define FONT_H

typedef struct mach_font_t mach_font_t;
typedef struct mach_font_lib_t mach_font_lib_t;
typedef struct mach_logger_t mach_logger_t;
typedef struct mach_renderer_t mach_renderer_t;

mach_font_t *mach_font_create(mach_font_lib_t *lib, mach_logger_t *logger,
		mach_renderer_t *renderer, const char *font_path, int font_size);
void mach_font_draw_text(mach_font_t *self, int x, int y, unsigned int color,
		const char *text);
void mach_font_destroy(mach_font_t *self);

#endif

