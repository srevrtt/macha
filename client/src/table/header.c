#include <stdlib.h>

#include <logger.h>
#include <renderer.h>

#include "include/header.h"

struct mach_table_header_t {
    int num_columns;
};

mach_table_header_t *mach_table_header_create(mach_logger_t *logger,
        mach_renderer_t *renderer) {
    mach_table_header_t *header = malloc(sizeof(mach_table_header_t));

    header->num_columns = 1;

    return header;
}

void mach_table_header_destroy(mach_table_header_t *self) {
    if (self) {
        free(self);
    }
}

