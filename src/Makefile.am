# Copyright (C) 2004,2009,2013 Remik Ziemlinski <first d0t surname att n0aa d0t g0v>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS	= nccmp

nccmp_SOURCES	= common.h \
nccmp.c nccmp.h \
ncinfo.c ncinfo.h opt.c opt.h \
strlist.c strlist.h xmalloc.c xmalloc.h

if SYSTEM_GETOPT
nccmp_SOURCES += 
else
nccmp_SOURCES += getopt.c getopt.h 
endif

nccmp_CFLAGS	= -I.. $(AM_CFLAGS)

nccmp_LDADD	= -lnetcdf -lm
