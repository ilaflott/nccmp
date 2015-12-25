#ifndef NCCMP_IO_H
#define NCCMP_IO_H

#include <log.h>
#include <netcdf.h>

#define NCFORMATSTR(f)                                                         \
    (f == NC_FORMAT_CLASSIC ? "NC_FORMAT_CLASSIC" :                            \
    (f == NC_FORMAT_64BIT   ? "NC_FORMAT_64BIT"   :                            \
    (f == NC_FORMAT_NETCDF4 ? "NC_FORMAT_NETCDF4" :                            \
                              "NC_FORMAT_NETCDF4_CLASSIC")))

#define DEBUG_COLOR    YELLOW "DEBUG"  NOCOLOR
#define DIFFER                "DIFFER"
#define DIFFER_COLOR   RED    "DIFFER" NOCOLOR

#define DIFF_MSG            " : VARIABLE : %s : POSITION : [%s] : VALUES : %s <> %s\n"
#define DIFF_MSG_PCT        " : VARIABLE : %s : POSITION : [%s] : VALUES : %s <> %s : PERCENT : %g\n"
#define DIFF_MSG_FIELD      " : VARIABLE : %s : POSITION : [%s] : FIELD : %s : VALUES : %s <> %s\n"
#define DIFF_MSG_FIELD_VLEN " : VARIABLE : %s : POSITION : [%s] : FIELD : %s : VARIABLE LENGTH : %zu <> %zu\n"
#define DIFF_MSG_VLEN       " : VARIABLE : %s : POSITION : [%s] : VARIABLE LENGTH : %zu <> %zu\n"
#define DIFF_MSG_PCT_FIELD  " : VARIABLE : %s : POSITION : [%s] : FIELD : %s : VALUES : %s <> %s : PERCENT : %g\n"

#if HAVE_PTHREAD
    #include <pthread.h>
    extern pthread_mutex_t mutex_print;
    #define PRINT_FUN(FUN, ...) {                                              \
        pthread_mutex_lock(& mutex_print);                                     \
        FUN(__VA_ARGS__);                                                      \
        pthread_mutex_unlock(& mutex_print);                                   \
    }
    #define PRINT_SETUP() pthread_mutex_t mutex_print;
#else
    #define PRINT_FUN(FUN, ...) FUN(__VA_ARGS__);
    #define PRINT_SETUP() /* empty */
#endif

/* Always print diffs to stderr. */
#define PRINT_DIFF(COLOR, DBG, FMT, ...)                                       \
    if (DBG) {                                                                 \
        if (COLOR) {                                                           \
            PRINT_FUN(LOG_DEBUG_CHOOSE_STDERR, COLOR, DIFFER_COLOR FMT, __VA_ARGS__); \
        } else {                                                               \
            PRINT_FUN(LOG_STREAM, "DEBUG", stderr, DIFFER FMT, __VA_ARGS__);   \
        }                                                                      \
    } else {                                                                   \
        if (COLOR) {                                                           \
            PRINT_FUN(fprintf, stderr, DIFFER_COLOR FMT, __VA_ARGS__);         \
        } else {                                                               \
            PRINT_FUN(fprintf, stderr, DIFFER FMT, __VA_ARGS__);               \
        }                                                                      \
    }

void io_destroy();
void io_init();

#endif
