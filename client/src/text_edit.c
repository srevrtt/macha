#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/text.h"
#include "include/text_edit.h"

struct mach_text_edit_t {
    int x;
    int y;
    int w;
    int h;

    bool multi_line;
    bool selected;

    char *text_buffer;
    int max_text_len;

    mach_text_t **lines;
};

mach_text_edit_t *mach_text_edit_create(int x, int y, int multi_line,
        int max_text_len, int w, int h) {
    mach_text_edit_t *self = malloc(sizeof(mach_text_edit_t));

    self->x = x;
    self->y = y;
    self->w = w;
    self->h = h;
    self->max_text_len = max_text_len;

    self->multi_line = multi_line;
    self->selected = true;

    self->text_buffer = malloc(max_text_len);

    if (multi_line) {
        self->lines = malloc(4 * sizeof(void*)); // start with 4 lines
        return self;
    }

    self->lines = malloc(sizeof(void*));

    return self;
}

void mach_text_edit_update(mach_text_edit_t *self) {
    if (self->selected) {
        // TODO
    }
}

/* note: this function does allocate the text buffer passed into it, so it can
   be NULL */
void mach_text_edit_get_content(mach_text_edit_t *self, char *text, int *len) {
    *len = strlen(self->text_buffer);
    text = malloc(*len);
    strncpy(text, self->text_buffer, self->max_text_len);
}

void mach_text_edit_destroy(mach_text_edit_t *self) {
    if (self) {
        free(self);
    }
}

