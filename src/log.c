#include <stdio.h>
#include <stdarg.h>

#include "log.h"

const char *loglevel_str[] = {
	"debug",
	"info",
	"warn",
	"error",
	"fatal",
	"exit"
};

void kvmkvm_log(log_level level, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "[kvmkvm/%s]: ", loglevel_str[level]);
	vfprintf(stderr, fmt, args);
	va_end(args);
}