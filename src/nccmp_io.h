#ifndef NCCMP_IO_H
#define NCCMP_IO_H

#include <log.h>
#include <netcdf.h>

#define NCFORMATSTR(f)                                                         \
    (f == NC_FORMAT_CLASSIC ? "NC_FORMAT_CLASSIC" :                            \
    (f == NC_FORMAT_64BIT   ? "NC_FORMAT_64BIT"   :                            \
    (f == NC_FORMAT_NETCDF4 ? "NC_FORMAT_NETCDF4" :                            \
                              "NC_FORMAT_NETCDF4_CLASSIC")))

#define DIFF_MSG            "DIFFER : VARIABLE : %s : POSITION : [%s] : VALUES : %s <> %s\n"
#define DIFF_MSG_PCT        "DIFFER : VARIABLE : %s : POSITION : [%s] : VALUES : %s <> %s : PERCENT : %g\n"
#define DIFF_MSG_FIELD      "DIFFER : VARIABLE : %s : POSITION : [%s] : FIELD : %s : VALUES : %s <> %s\n"
#define DIFF_MSG_FIELD_VLEN "DIFFER : VARIABLE : %s : POSITION : [%s] : FIELD : %s : VARIABLE LENGTH : %zu <> %zu\n"
#define DIFF_MSG_VLEN       "DIFFER : VARIABLE : %s : POSITION : [%s] : VARIABLE LENGTH : %zu <> %zu\n"
#define DIFF_MSG_PCT_FIELD  "DIFFER : VARIABLE : %s : POSITION : [%s] : FIELD : %s : VALUES : %s <> %s : PERCENT : %g\n"

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

/* Use synchronized debug printing if threaded. */
#define PRINT(DBG, ...)                                                        \
    if (DBG) {                                                                 \
        PRINT_FUN(LOG_STREAM, "DEBUG", stdout, __VA_ARGS__);                   \
    } else {                                                                   \
        PRINT_FUN(fprintf, stdout, __VA_ARGS__)                                \
    };

/* Always print diffs to stderr. */
#define PRINT_DIFF(DBG, ...)                                                   \
    if (DBG) {                                                                 \
        PRINT_FUN(LOG_STREAM, "DEBUG", stderr, __VA_ARGS__);                   \
    } else {                                                                   \
        PRINT_FUN(fprintf, stderr, __VA_ARGS__)                                \
    };


void io_destroy();
void io_init();

#endif
