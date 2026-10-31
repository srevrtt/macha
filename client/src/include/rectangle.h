#ifndef RECTANGLE_H
#define RECTANGLE_H

typedef struct mach_rectangle_t mach_rectangle_t;

mach_rectangle_t *mach_rectangle_create();
void mach_rectangle_draw(mach_rectangle_t *self);
void mach_rectangle_destroy(mach_rectangle_t *self);

#endif

