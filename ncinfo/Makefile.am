# Copyright (C) 2004,2009,2010 Remik Ziemlinski <first d0t surname att n0aa d0t g0v>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS = ncinfo
ncinfo_SOURCES = ncinfo.c
ncinfo_CFLAGS = -Iinclude
ncinfo_LDADD = -lnetcdf -lhdf5_hl -lhdf5 -lz -lm

CLEANFILES = test*.nc ncinfo

TESTS = test.sh 

EXTRA_DIST = test.sh test1.ncgen test2.ncgen test.stdout