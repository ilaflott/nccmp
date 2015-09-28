#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

void vlog_format(const char* tag, FILE * stream, const char* file, int line, ...);

#define LOG_DEBUG(...) { vlog_format("DEBUG", stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_ERROR(...) { vlog_format("ERROR", stderr, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_INFO(...)  { vlog_format("INFO",  stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_WARN(...)  { vlog_format("WARN",  stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_STREAM(LEVEL, STREAM, ...)  { vlog_format(LEVEL, STREAM, __FILE__, __LINE__, __VA_ARGS__); }

#endif
