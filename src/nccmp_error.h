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

#endif
