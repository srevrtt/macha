#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/app.h"
#include "include/window.h"
#include "include/logger.h"

struct mach_app_t {
    mach_logger_t *logger;
    mach_window_t *window;
};

mach_app_t *mach_app_create() {
    mach_app_t *app = malloc(sizeof(mach_app_t));
    if (app == NULL) {
        fprintf(stderr, "CRITICAL: Failed to allocate memory for the Macha "
                "Application struct! Exiting.\n");
        exit(EXIT_FAILURE);
    }

    /* initialize the logger */
#ifdef DEBUG_BUILD
    app->logger = mach_logger_create(
        "macha_client_debug.log", true);
#else
    app->logger = mach_logger_create("macha_client.log",
        false);
#endif

    /* create the window */
    mach_logger_log(app->logger, MACH_LOG_INFO, "Creating window..");
    app->window = mach_window_create(app->logger, MACH_WINDOW_WIDTH,
            MACH_WINDOW_HEIGHT, MACH_WINDOW_TITLE);

    /* window error handling (if applicable) */
    char *window_create_flag = mach_logger_get_flag(app->logger, NULL);
    if (window_create_flag && strcmp(window_create_flag,
                "WINDOW_CREATION_FAILED")) {
        mach_logger_log(app->logger, MACH_LOG_ERROR,
                "Cannot proceed; native Window creation failed!");
        mach_logger_destroy(app->logger);
    }

    mach_logger_log(app->logger, MACH_LOG_INFO,
            "Window created successfully");

    return app;
}

void mach_app_run(mach_app_t *self) {
    while (!mach_window_should_close(self->window)) {
        mach_window_update(self->window);
    }
}

void mach_app_destroy(mach_app_t *self) {
    if (self->window) {
        mach_window_destroy(self->window);
        mach_logger_destroy(self->logger);
    }
}

