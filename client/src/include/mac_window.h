#ifndef MAC_WINDOW_H
#define MAC_WINDOW_H

typedef struct mach_logger_t mach_logger_t;
typedef struct mach_mac_window_t mach_mac_window_t;

mach_mac_window_t *mach_mac_window_create(mach_logger_t *logger, int width, int height, char *title);
void mach_mac_window_gl_init(mach_logger_t *logger, mach_mac_window_t *window);
void mach_mac_window_gl_swap(mach_mac_window_t *window);
int mach_mac_window_should_close(mach_mac_window_t *window);
void mach_mac_window_fb_size(mach_mac_window_t *window, int *width, int *height);
void mach_mac_window_poll(mach_mac_window_t *window);
void mach_mac_window_destroy(mach_mac_window_t *window);

#endif

