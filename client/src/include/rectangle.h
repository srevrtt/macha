#ifndef RECTANGLE_H
#define RECTANGLE_H

typedef struct mach_rectangle_t mach_rectangle_t;
typedef struct mach_shader_t mach_shader_t;

mach_rectangle_t *mach_rectangle_create(int x, int y, int w, int h,
        int corner_radius);
void mach_rectangle_transform(mach_rectangle_t *self, int x, int y, int w,
		int h);
void mach_rectangle_draw(mach_rectangle_t *self, mach_shader_t *shader);
void mach_rectangle_destroy(mach_rectangle_t *self);

#endif

