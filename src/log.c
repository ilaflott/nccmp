#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

/* Prints: 2015-07-16 16:29:41.607984 -0400 INFO  ...blah.. */
void log_format(const char* tag, FILE * stream, const char* file,
                int line, const char* message, va_list args)
{
    const int BUF_LEN = 256;
    char            fmt[BUF_LEN], buffer[BUF_LEN];
    struct timeval  tv;
    struct tm       *tm;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    if(NULL != tm)
    {
        strftime(fmt, BUF_LEN, "%Y-%m-%d %H:%M:%S.%%06u %z", tm);
        snprintf(buffer, BUF_LEN, fmt, tv.tv_usec);
        fprintf(stream, "%s ", buffer);
    }
    fprintf(stream, "%s %s:%d ", tag, file, line);
    vfprintf(stream, message, args);
}

void vlog_format(const char* tag, FILE * stream, const char* file, int line, ...)
{
    va_list args;
    va_start(args, line);
    const char* message = va_arg(args, const char*);
    log_format(tag, stream, file, line, message, args);
    va_end(args);
}

