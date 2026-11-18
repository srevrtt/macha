#include <stdlib.h>

#include <logger.h>
#include <renderer.h>

#include "include/header.h"

struct mach_table_header_t {
    mach_table_column_t **columns;
    int num_columns;
    size_t columns_arr_size;
};

mach_table_header_t *mach_table_header_create(mach_logger_t *logger,
        mach_renderer_t *renderer) {
    mach_table_header_t *header = malloc(sizeof(mach_table_header_t));

    header->num_columns = 1;
    header->columns_arr_size = 2;
    header->columns = malloc(header->columns_arr_size * sizeof(void*));

    mach_table_header_new_column(header, "Column 1",
            MACH_TABLE_COLUMN_TYPE_TEXT, "");

    return header;
}

void mach_table_header_new_column(mach_table_header_t *self,
        const char *col_name, mach_table_column_type_t col_type,
        const char *col_formula) {
    /* resize the array if needed */
    if (self->num_columns >= self->columns_arr_size) {
        self->columns_arr_size *= 2;
        mach_table_column_t **temp = realloc(self->columns,
                self->columns_arr_size * sizeof(void*));

        if (temp) {
            self->columns = temp;
        }
    }

    /* add the column */
    self->columns[self->num_columns++] = mach_table_column_create(col_name,
            col_type, col_formula);
}

void mach_table_header_destroy(mach_table_header_t *self) {
    if (self) {
        free(self->columns);
        free(self);
    }
}

