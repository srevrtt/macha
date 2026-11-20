#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/column.h"

struct mach_table_column_t {
    mach_table_column_type_t type;
    char *formula;
    char *name;

    union {
        int dollars;
        char cents; // 1 byte number
        bool boolean;
        char *text;
    } data;
};

mach_table_column_t *mach_table_column_create(const char *name,
        mach_table_column_type_t type, const char *formula) {
    mach_table_column_t *column = malloc(sizeof(mach_table_column_t));

    /* initialize the column data */
    // TODO: consider a resizable string for memory usage limitation
    column->formula = malloc(512 * sizeof(char));
    column->name = malloc(128 * sizeof(char));

    /* insert the data */
    mach_table_column_update(column, name, type, formula);

    return column;
}

/* note: any argument passed to this function can be NULL (or -1 respectfully),
   except for `self`, in order to control which parameters update rather than
   updating all of them at the same time */
void mach_table_column_update(mach_table_column_t *self, const char *name,
        mach_table_column_type_t type, const char *formula) {
    if (name) {
        /* name can be a maximum of 128 bytes total */
        strncpy(self->name, name, 127);
    }

    if (formula) {
        /* formula can be a maximum of 512 bytes total */
        strncpy(self->formula, formula, 511);
    }

    if (type > -1) {
        self->type = type;
    }
}

void mach_table_column_destroy(mach_table_column_t *self) {
    if (self) {
        free(self->formula);
        free(self->name);
        free(self);
    }
}

