#ifndef WINDOW_H
#define WINDOW_H

#if defined(__APPLE__) && defined(__MACH__)
typedef struct mach_mac_window_t mach_mac_window_t;
#endif

typedef struct {
	int width;
	int height;
	int should_close;
#if defined(__APPLE__) && defined(__MACH__)
	mach_mac_window_t *handle;
#endif
} mach_window_t ;


mach_window_t *mach_window_create(int width, int height, char *title);
void mach_window_update(mach_window_t *self);
void mach_window_destroy(mach_window_t *self);

#endif

