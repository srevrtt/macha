#ifndef TEXT_EDIT_H
#define TEXT_EDIT_H

typedef struct mach_text_edit_t mach_text_edit_t;
typedef struct mach_logger_t mach_logger_t;

mach_text_edit_t *mach_text_edit_create(int x, int y, int multi_line,
        int max_text_len, int w, int h);
void mach_text_edit_update(mach_text_edit_t *self);
void mach_text_edit_get_content(mach_text_edit_t *self, char *text, int *len);
void mach_text_edit_destroy(mach_text_edit_t *self);

#endif

