#include <stdlib.h>
#include <stdbool.h>

#include <glad/glad.h>

#include "include/logger.h"
#include "include/mac_window.h"
#include "include/window.h"

struct mach_window_t {
    int width;
    int height;
    int should_close;
#if defined(__APPLE__) && defined(__MACH__)
    mach_mac_window_t *handle;
#endif
};

mach_window_t *mach_window_create(mach_logger_t *logger, int width, int height, char *title) {
    mach_window_t *window = malloc(sizeof(mach_window_t));

    window->width = width;
    window->height = height;
    window->should_close = false;

#if defined(__APPLE__) && defined(__MACH__)
    window->handle = mach_mac_window_create(logger, width, height, title);
    mach_mac_window_gl_init(logger, window->handle);
#endif

    /* load OpenGL functions */
    gladLoadGL();

    return window;
}

int mach_window_should_close(mach_window_t *self) {
    return self->should_close;
}

void mach_window_get_size(mach_window_t *self, int *width, int *height) {
#if defined(__APPLE__) && defined(__MACH__)
    mach_mac_window_fb_size(self->handle, width, height);
#endif
}

/* performs a window-wide event polling operation; called every frame */
void mach_window_update(mach_window_t *self) {
#if defined(__APPLE__) && defined(__MACH__)
    mach_mac_window_poll(self->handle);
    self->should_close = mach_mac_window_should_close(self->handle);
#endif
}

void mach_window_swap_buffers(mach_window_t *self) {
#if defined(__APPLE__) && defined(__MACH__)
    mach_mac_window_gl_swap(self->handle);
#endif
}

/* frees and destroys the window object w/ double-free checking */
void mach_window_destroy(mach_window_t *self) {
#if defined(__APPLE__) && defined(__MACH__)
    mach_mac_window_destroy(self->handle);
#endif
}

