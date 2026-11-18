#ifndef TABLE_HEADER_H
#define TABLE_HEADER_H

typedef struct mach_table_header_t mach_table_header_t;
typedef struct mach_logger_t mach_logger_t;
typedef struct mach_renderer_t mach_renderer_t;

mach_table_header_t *mach_table_header_create(mach_logger_t *logger,
    mach_renderer_t *renderer);
void mach_table_header_destroy(mach_table_header_t *self);

#endif

