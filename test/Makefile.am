# Copyright (C) 2004,2009 Remik Ziemlinski <first d0t surname att n0aa d0t g0v>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = test*.nc stderr.tmp padded.nc unpadded.nc test_strlist

TESTS = nccmp_test.sh 

EXTRA_DIST = nccmp_test.sh test1a.cdl test1b.cdl \
test2a.cdl test2b.cdl test3a.cdl test3b.cdl \
stderr1.txt stderr2.txt stderr3.txt stderr4.txt stderr5.txt \
stderr6.txt stderr7.txt stderr8.txt 

check_PROGRAMS = padheader test_strlist
padheader_LDADD = -lnetcdf
test_strlist_CPPFLAGS = -I../src
test_strlist_LDADD = ../src/nccmp-xmalloc.o ../src/nccmp-strlist.o
