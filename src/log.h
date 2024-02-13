#ifndef LOG_H
#define LOG_H

typedef enum {
	LOG_DEBUG = 0,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
	LOG_EXIT
} log_level;

void kvmkvm_log(log_level level, const char *fmt, ...);

#endif /* LOG_H */
