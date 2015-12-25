#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

void vlog_format(const char* tag, FILE * stream, const char* file, int line, ...);

#define NOCOLOR "\033[0m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define MAGENTA "\033[0;35m"

#define LOG_STREAM(LEVEL, STREAM, ...)  { vlog_format(LEVEL, STREAM, __FILE__, __LINE__, __VA_ARGS__); }

#define LOG_DEBUG(...) { vlog_format("DEBUG", stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_DEBUG_COLOR(...) { vlog_format(YELLOW "DEBUG" NOCOLOR, stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_DEBUG_CHOOSE(COLOR, ...) if (COLOR) { LOG_DEBUG_COLOR(__VA_ARGS__); } else { LOG_DEBUG(__VA_ARGS__); }
#define LOG_DEBUG_CHOOSE_STDERR(COLOR, ...) if (COLOR) { LOG_STREAM(YELLOW "DEBUG" NOCOLOR, stderr, __VA_ARGS__) } else { LOG_STREAM("DEBUG", stderr, __VA_ARGS__) }

#define LOG_ERROR(...) { vlog_format("ERROR", stderr, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_ERROR_COLOR(...) { vlog_format(RED "ERROR" NOCOLOR, stderr, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_ERROR_CHOOSE(COLOR, ...) if (COLOR) { LOG_ERROR_COLOR(__VA_ARGS__); } else { LOG_ERROR(__VA_ARGS__); }

#define LOG_INFO(...)  { vlog_format("INFO",  stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_INFO_COLOR(...)  { vlog_format(GREEN "INFO" NOCOLOR,  stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_INFO_CHOOSE(COLOR, ...) if (COLOR) { LOG_INFO_COLOR(__VA_ARGS__); } else { LOG_INFO(__VA_ARGS__); }

#define LOG_WARN(...)  { vlog_format("WARN",  stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_WARN_COLOR(...)  { vlog_format(MAGENTA "WARN" NOCOLOR,  stdout, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_WARN_CHOOSE(COLOR, ...) if (COLOR) { LOG_WARN_COLOR(__VA_ARGS__); } else { LOG_WARN(__VA_ARGS__); }


#endif
