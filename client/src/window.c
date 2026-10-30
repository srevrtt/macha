#include <stdlib.h>
#include <stdbool.h>

#include "include/mac_window.h"

#include "include/window.h"

mach_window_t *mach_window_create(int width, int height, char *title) {
	mach_window_t *window = malloc(sizeof(mach_window_t));

	window->width = width;
	window->height = height;
	window->should_close = false;

#if defined(__APPLE__) && defined(__MACH__)
	window->handle = mach_mac_window_create(width, height, title);
#endif

	return window;
}

/* performs a window-wide event polling operation; called every frame */
void mach_window_update(mach_window_t *self) {
#if defined(__APPLE__) && defined(__MACH__)
	mach_mac_window_poll(self->handle);
	self->should_close = mach_mac_window_should_close(self->handle);
#endif
}

/* frees and destroys the window object w/ double-free checking */
void mach_window_destroy(mach_window_t *self) {
#if defined(__APPLE__) && defined(__MACH__)
	mach_mac_window_destroy(self->handle);
#endif
}

