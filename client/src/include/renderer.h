#ifndef RENDERER_H
#define RENDERER_H

typedef struct mach_logger_t mach_logger_t;
typedef struct mach_renderer_t mach_renderer_t;

mach_renderer_t *mach_renderer_create(int width, int height,
		mach_logger_t *logger);
void mach_renderer_draw(mach_renderer_t *self);
void mach_renderer_destroy(mach_renderer_t *self);

#endif

