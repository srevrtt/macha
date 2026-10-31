#ifndef SHADER_H
#define SHADER_H

#include <sys/wait.h>
typedef struct mach_logger_t mach_logger_t;
typedef struct mach_shader_t mach_shader_t;
typedef float vec3[3];
typedef float mat4[4][4];

mach_shader_t *mach_shader_create(mach_logger_t *logger,
        unsigned char *vertex_src, unsigned char *fragment_src,
        unsigned int vertex_len, unsigned int fragment_len);
void mach_shader_mat4(mach_shader_t *self, const char *name, mat4 m);
void mach_shader_vec3(mach_shader_t *self, const char *name, vec3 v);
void mach_shader_use(mach_shader_t *self);
void mach_shader_destroy(mach_shader_t *self);

#endif

