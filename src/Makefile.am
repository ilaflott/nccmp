# Copyright (C) 2004,2009,2013 Remik Ziemlinski <first d0t surname att n0aa d0t g0v>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS     = nccmp

CLEANFILES       = nccmp nccmp.exe

nccmp_SOURCES    =                              \
    common.h                                    \
    log.c log.h                                 \
    nccmp.c             nccmp.h                 \
    xmalloc.c           xmalloc.h               \
    nccmp_data.c        nccmp_data.h            \
    nccmp_dim.c         nccmp_dim.h             \
    nccmp_error.h                               \
    nccmp_format.c      nccmp_format.h          \
    nccmp_group.c       nccmp_group.h           \
    nccmp_io.c          nccmp_io.h              \
    nccmp_metadata.c    nccmp_metadata.h        \
    nccmp_nc_type.c     nccmp_nc_type.h         \
    nccmp_ncinfo.c      nccmp_ncinfo.h          \
    nccmp_nest.c        nccmp_nest.h            \
    nccmp_odometer.c    nccmp_odometer.h        \
    nccmp_opt.c         nccmp_opt.h             \
    nccmp_state.c       nccmp_state.h           \
    nccmp_strlist.c     nccmp_strlist.h         \
    nccmp_thread.h                              \
    nccmp_utils.c       nccmp_utils.h           \
    nccmp_user_type.c   nccmp_user_type.h       \
    nccmp_var.c         nccmp_var.h

if SYSTEM_GETOPT
nccmp_SOURCES += 
else
nccmp_SOURCES += getopt.c getopt.h 
endif

nccmp_CFLAGS    = -I.. $(AM_CFLAGS)

nccmp_LDADD     = -lnetcdf -lm 

