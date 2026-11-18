#ifndef COLUMN_H
#define COLUMN_H

typedef enum {
    MACH_TABLE_COLUMN_TYPE_DOLLARS,
    MACH_TABLE_COLUMN_TYPE_BOOL,
    MACH_TABLE_COLUMN_TYPE_TEXT
} mach_table_column_type_t;

typedef struct mach_table_column_t mach_table_column_t;

mach_table_column_t *mach_table_column_create(const char *name,
        mach_table_column_type_t type, const char *formula);
void mach_table_column_update(mach_table_column_t *self, const char *name,
        mach_table_column_type_t type, const char *formula);
void mach_table_column_destroy(mach_table_column_t *self);

#endif

