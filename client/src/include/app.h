#ifndef APP_H
#define APP_H

#ifndef NDEBUG
#define DEBUG_BUILD
#endif

#define MACH_WINDOW_WIDTH 800
#define MACH_WINDOW_HEIGHT 600
#define MACH_WINDOW_TITLE "Macha v0.1.0"

typedef struct mach_app_t mach_app_t;

mach_app_t *mach_app_create();
void mach_app_run(mach_app_t *self);
void mach_app_destroy(mach_app_t *self);

#endif

