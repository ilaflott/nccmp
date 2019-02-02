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

//Netcdf 4.6.2: NC_EFILTER Variable is not szip encoded.
#define HANDLE_NC_SZIP_ERROR(STATUS) {                                         \
    if (NC_NOERR != STATUS && NC_EFILTER != STATUS) {                          \
        LOG_ERROR(nc_strerror(STATUS));                                        \
        exit(-1);                                                              \
    }                                                                          \
}

#endif
