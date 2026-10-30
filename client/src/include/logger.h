#ifndef LOGGER_H
#define LOGGER_H

#define MAX_LOG_SIZE 2048

typedef struct mach_logger_t mach_logger_t;

typedef enum {
	MACH_LOG_INFO,
	MACH_LOG_WARN,
	MACH_LOG_ERROR,
	MACH_LOG_CRITICAL
} mach_log_level_t;

mach_logger_t *mach_logger_create(const char *log_path, int log_to_console);
void mach_logger_log(mach_logger_t *self, mach_log_level_t level,
		const char *fmt, ...);
void mach_logger_destroy(mach_logger_t *self);

#endif

