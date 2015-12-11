#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define PASS 0
#define FAIL 1
extern int failures;

void vprint_args(FILE * stream, const char* file, int line,
            const char* message, va_list args)
{
    fprintf(stream, "%s:%d ", file, line);
    vfprintf(stream, message, args);
    fflush(stream);
}

void vprint(FILE * stream, const char* file, int line, ...)
{
    va_list args;
    va_start(args, line);
    const char* message = va_arg(args, const char*);
    vprint_args(stream, file, line, message, args);
    va_end(args);
}

#define PRINT(...) { vprint(stdout, __FILE__, __LINE__, __VA_ARGS__); }

#define TEST(FUN) {                   \
    PRINT(#FUN ": ");                 \
    if (PASS == FUN()) {              \
        printf("PASS\n");             \
    } else {                          \
        printf("FAIL\n");             \
        ++failures;                   \
    }                                 \
}

#define ROUND(V, N) ( (int)((V) * pow(10.0, (N))) / pow(10.0, (N)) )
#define ALMOST_EQUAL(A, B, EPS) ( abs((A)-(B)) < EPS )

#endif /* TEST_COMMON_H */
