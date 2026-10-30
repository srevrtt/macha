#ifndef WINDOW_H
#define WINDOW_H

typedef struct mach_logger_t mach_logger_t;
typedef struct mach_window_t mach_window_t;

mach_window_t *mach_window_create(mach_logger_t *logger, int width, int height,
		char *title);
int mach_window_should_close(mach_window_t *self);
void mach_window_update(mach_window_t *self);
void mach_window_destroy(mach_window_t *self);

#endif

