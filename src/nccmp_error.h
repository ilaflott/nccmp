#ifndef NCCMP_ERROR_H
#define NCCMP_ERROR_H

#include <stdlib.h>
#include <netcdf.h>
#include <log.h>

#define HANDLE_NC_ERROR(STATUS) {                                              \
    if (NC_NOERR != STATUS) {                                                  \
        LOG_ERROR(nc_strerror(STATUS));                                        \
        exit(-1);                                                              \
    }                                                                          \
}

//NC_EFILTER: Variable is not szip encoded.
#ifdef NC_EFILTER
    #define IS_NC_EFILTER(STATUS) (NC_EFILTER == STATUS)
#else
    #define IS_NC_EFILTER(STATUS) (0)
#endif

#define HANDLE_NC_SZIP_ERROR(STATUS) {                                         \
    if (NC_NOERR != STATUS && !IS_NC_EFILTER(STATUS)) {                        \
        LOG_ERROR(nc_strerror(STATUS));                                        \
        exit(-1);                                                              \
    }                                                                          \
}

#endif
