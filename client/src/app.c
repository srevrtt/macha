#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/app.h"
#include "include/window.h"
#include "include/logger.h"
#include "include/renderer.h"
#include "include/rectangle.h"

struct mach_app_t {
    mach_logger_t *logger;
    mach_window_t *window;
    mach_renderer_t *renderer;
    mach_rectangle_t *test_rect;
    mach_rectangle_t *test_rect2;
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

    /* before creating the renderer, we need to get the actual, usable
       rendering width and height, mostly for Apple Retina screens */
    int width = 0, height = 0;
    mach_window_get_size(app->window, &width, &height);

    /* create and initialize the renderer */
    mach_logger_log(app->logger, MACH_LOG_INFO,
            "Initializing the application renderer..");
    app->renderer = mach_renderer_create(width, height, app->logger);

    /* handle if something happened and the renderer failed to initialize */
    if (app->renderer == NULL) {
        mach_logger_log(app->logger, MACH_LOG_CRITICAL,
                "Failed to initialize the application renderer");
        return NULL;
    }

    /* otherwise, we're all good! */
    mach_logger_log(app->logger, MACH_LOG_INFO,
            "The application renderer has been initialized successully");

    /* now, we can create all of the UI objects */
    mach_logger_log(app->logger, MACH_LOG_INFO,
            "Initializing the UI..");

    app->test_rect = mach_rectangle_create(10, 10, 300, 300);
    mach_renderer_add_rect(app->renderer, app->logger,
            app->test_rect);

    app->test_rect2 = mach_rectangle_create(320, 10, 300, 300);
    mach_renderer_add_rect(app->renderer, app->logger,
            app->test_rect2);

    return app;
}

void mach_app_run(mach_app_t *self) {
    while (!mach_window_should_close(self->window)) {
        mach_window_update(self->window);

        /* draw the objects on the back buffer */
        mach_renderer_draw(self->renderer);

        /* swap the buffers to render the objects */
        mach_window_swap_buffers(self->window);
    }
}

void mach_app_destroy(mach_app_t *self) {
    if (self) {
        mach_renderer_destroy(self->renderer);
        mach_window_destroy(self->window);
        mach_logger_destroy(self->logger);
        free(self);
    }
}

