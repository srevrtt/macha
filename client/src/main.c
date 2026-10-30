#include <stdio.h>
#include <stdbool.h>

#include "include/window.h"
#include "include/logger.h"

#ifndef NDEBUG
#define DEBUG_BUILD
#endif

int main(int argc, char *argv[]) {
    /* initialize the logger */
#ifdef DEBUG_BUILD
    mach_logger_t *logger = mach_logger_create(
            "macha_client_debug.log", true);
#else
    mach_logger_t *logger = mach_logger_create("macha_client.log",
            false);
#endif

	mach_window_t *window = mach_window_create(800, 600,
            "Macha v0.1.0");

	while (!window->should_close) {
		mach_window_update(window);
	}

	mach_window_destroy(window);
    mach_logger_destroy(logger);

	return 0;
}

