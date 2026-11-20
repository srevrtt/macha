#include <stdlib.h>

#include <logger.h>
#include <renderer.h>
#include <rectangle.h>

#include "include/header.h"

#define HEADER_UI_MARGIN 10
#define CELL_PADDING 3

struct mach_table_header_t {
    mach_table_column_t **columns;
    int num_columns;
    size_t columns_arr_size;

    /* used for drawing the UI */
    int current_x;
};

mach_table_header_t *mach_table_header_create(mach_logger_t *logger,
        mach_renderer_t *renderer) {
    mach_table_header_t *header = malloc(sizeof(mach_table_header_t));

    header->num_columns = 1;
    header->columns_arr_size = 2;
    header->columns = malloc(header->columns_arr_size * sizeof(void*));

    header->current_x = HEADER_UI_MARGIN;

    /* create the default columns */
    mach_table_header_new_column(header, "Column 1",
            MACH_TABLE_COLUMN_TYPE_TEXT, "",
            logger, renderer);
    mach_table_header_new_column(header, "Column 2",
            MACH_TABLE_COLUMN_TYPE_TEXT, "",
            logger, renderer);
    mach_table_header_new_column(header, "Column 3",
            MACH_TABLE_COLUMN_TYPE_TEXT, "",
            logger, renderer);


    return header;
}

void mach_table_header_new_column(mach_table_header_t *self,
        const char *col_name, mach_table_column_type_t col_type,
        const char *col_formula, mach_logger_t *logger,
        mach_renderer_t *renderer) {
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
    self->columns[self->num_columns++] =
        mach_table_column_create(col_name, col_type,
                col_formula);

    /* create the rectangle to actually render this */
    const int cell_width = 300;
    const int cell_height = 70;

    mach_rectangle_t *rect = mach_rectangle_create(self->current_x, 10, cell_width, cell_height, 4, 0xB93E3E);
    mach_renderer_add_rect(renderer, logger, rect);

    /* now advance the current_x "pointer" */
    self->current_x += cell_width + CELL_PADDING;
}

void mach_table_header_destroy(mach_table_header_t *self) {
    if (self) {
        free(self->columns);
        free(self);
    }
}

