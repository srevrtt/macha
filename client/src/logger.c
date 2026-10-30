#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "include/logger.h"

struct mach_logger_t {
    FILE *log_file;
	bool write_to_console;
};

mach_logger_t *mach_logger_create(const char *log_path, int log_to_console) {
	mach_logger_t *logger = malloc(sizeof(mach_logger_t));
    logger->write_to_console = log_to_console;

    /* open a dummy file handle to clear the existing log file if it exists */
    FILE *dummy_fp = fopen(log_path, "w");

    if (dummy_fp == NULL) {
        fprintf(stderr, "CRITICAL: Failed to override log file \"%s\"!\n",
                log_path);
        free(logger);

        /* no need to gracefully exit as the logger is first to initialize */
        exit(EXIT_FAILURE);
    }

    if (fclose(dummy_fp) == EOF) {
        fprintf(stderr, "CRITICAL: Failed to close dummy log file pointer:"
                "\"%s\"!\n", log_path);
        free(logger);
        exit(EXIT_FAILURE);
    }

    /* now we can open the real one! */
    logger->log_file = fopen(log_path, "a");

    if (logger->log_file == NULL) {
        fprintf(stderr, "CRITICAL: Failed to open log file handle: \"%s\"!\n",
                log_path);
        free(logger);
        exit(EXIT_FAILURE);
    }

    mach_logger_log(logger, MACH_LOG_INFO,
            "Logger has initialized successfully!");

    return logger;
}

/* logs a formatted message to the log file and stdout (if enabled);
   there is a maximum of MAX_LOG_SIZE characters per message */
void mach_logger_log(mach_logger_t *self, mach_log_level_t level,
        const char *fmt, ...) {
    /* convert the formatted string into a real string buffer */

    char buffer[MAX_LOG_SIZE];
    va_list args;
    bool truncated = false;

    va_start(args, fmt);

    int written = vsnprintf(buffer, MAX_LOG_SIZE, fmt, args);
    if (written >= MAX_LOG_SIZE) {
        truncated = true;
    }

    va_end(args);

    /* map the log level enum to a string */
    char logLevelStr[9];
    switch (level) {
        case MACH_LOG_WARN:
            strcpy(logLevelStr, "WARN");
            break;
        case MACH_LOG_ERROR:
            strcpy(logLevelStr, "ERROR");
            break;
        case MACH_LOG_CRITICAL:
            strcpy(logLevelStr, "CRITICAL");
            break;
        /* also handles MACH_LOG_INFO */
        default:
            strcpy(logLevelStr, "INFO");
            break;
    }

    /* prepare the message */
    char message[MAX_LOG_SIZE + 25];

    /* I know this is very confusing, but it's really just a bunch of
       conditional formatting; it'll make sense eventually, I promise */
    snprintf(message, MAX_LOG_SIZE + 25, "%s%s: %s%c\n", truncated ?
            "[TRUNCATED] " : "", logLevelStr, buffer, (level == MACH_LOG_ERROR ||
                level == MACH_LOG_CRITICAL) ? '!' : '\0');

    /* now we can actually write it to the log file */
    size_t written_items = fwrite(message, sizeof(char),
            strlen(message), self->log_file);
    if (written_items != strlen(message)) {
        fprintf(stderr, "ERROR: Failed to write the log message below! Only "
                "wrote %zu characters out of %lu. Defaulting to stderr: \n%s\n",
                written_items, strlen(message), message);
        return;
    }

    /* append a newline */
    fputc('\n', self->log_file);

    /* now, write it to stdout or stderr if write_to_console is enabled */
    if (self->write_to_console) {
        if (level == MACH_LOG_ERROR || level == MACH_LOG_CRITICAL) {
            fputs(message, stderr);
            return;
        }

        puts(message);
    }
}

/* safely destroys and deallocates the logger */
void mach_logger_destroy(mach_logger_t *self) {
    if (self->log_file) {
        fclose(self->log_file);
        free(self);
        self = NULL;
    }
}

