#ifndef TABLE_HEADER_H
#define TABLE_HEADER_H

// TODO: have a separate header for `mach_table_column_type_t`
#include "column.h"

typedef struct mach_table_header_t mach_table_header_t;
typedef struct mach_logger_t mach_logger_t;
typedef struct mach_renderer_t mach_renderer_t;

mach_table_header_t *mach_table_header_create(mach_logger_t *logger, mach_renderer_t *renderer);
void mach_table_header_new_column(mach_table_header_t *self,
        const char *col_name, mach_table_column_type_t col_type,
        const char *col_formula, mach_logger_t *logger,
        mach_renderer_t *renderer);
void mach_table_header_destroy(mach_table_header_t *self);

#endif

