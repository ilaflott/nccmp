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

nccmp_SOURCES    =      \
    log.c               \
    nccmp.c             \
    xmalloc.c           \
    nccmp_data.c        \
    nccmp_darray.c      \
    nccmp_dim.c         \
    nccmp_format.c      \
    nccmp_group.c       \
    nccmp_io.c          \
    nccmp_metadata.c    \
    nccmp_nc_type.c     \
    nccmp_ncinfo.c      \
    nccmp_nest.c        \
    nccmp_odometer.c    \
    nccmp_opt.c         \
    nccmp_state.c       \
    nccmp_stats.c       \
    nccmp_stats_print.c \
    nccmp_strlist.c     \
    nccmp_utils.c       \
    nccmp_user_type.c   \
    nccmp_var.c

if SYSTEM_GETOPT
nccmp_SOURCES += 
else
nccmp_SOURCES += getopt.c 
endif

nccmp_CFLAGS    = -I$(top_srcdir) $(AM_CFLAGS)

nccmp_LDADD     = -lnetcdf -lm 

