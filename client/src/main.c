#include <stdio.h>

#include "include/window.h"

int main(int argc, char *argv[]) {
	mach_window_t *window = mach_window_create(800, 600, "Macha v0.1.0");

	while (!window->should_close) {
		mach_window_update(window);
	}

	mach_window_destroy(window);
	return 0;
}

