#ifndef MAC_WINDOW_H
#define MAC_WINDOW_H

typedef struct mach_logger_t mach_logger_t;
typedef struct mach_mac_window_t mach_mac_window_t;

mach_mac_window_t *mach_mac_window_create(mach_logger_t *logger, int width, int height, char *title);
int mach_mac_window_should_close(mach_mac_window_t *window);
void mach_mac_window_poll(mach_mac_window_t *window);
void mach_mac_window_destroy(mach_mac_window_t *window);

#endif

