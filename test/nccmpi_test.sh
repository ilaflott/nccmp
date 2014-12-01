#! /bin/sh

#   Copyright (C) 2004-2007 Remik Ziemlinski @ noaa gov
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; see the file COPYING.
#   If not, write to the Free Software Foundation,
#   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  


# find 'ncgen' command
NCGEN=`which ncgen 2> /dev/null`

if test "$?" = "0"; then :; else 
    echo "ERROR: ncgen command not found. Aborting tests..."
fi

# check for nccmp executable in source tree
NCCMP=`which ../src/nccmpi 2> /dev/null`

if test "$?" = "0"; then :; else
    echo "ERROR: nccmp not built or missing from ../src/"
fi
NCCMP="mpirun -np 2 $NCCMP"

# create netcdf test files
NCDL="3"
ctr=1

while test "$ctr" -le "$NCDL" 
do
    `ncgen -o test${ctr}a.nc test${ctr}a.cdl` 
    if test "$?" = "0"; then :; else
	exit 1
    fi

    `ncgen -o test${ctr}b.nc test${ctr}b.cdl` 
    if test "$?" = "0"; then :; else
	exit 1
    fi
    
    ctr=`expr $ctr + 1`
done

# 1 metadata globals
echo "1."
CMD="$NCCMP -mgqf -x data1,data2,data3,data4,recdata,rec test1a.nc test1b.nc"
$CMD > stderr.tmp 2> /dev/null
ERR=`cat stderr.tmp`

if test `echo -n $ERR` = "1"; then :; else
    echo
    echo "metadata comparison failed on global attributes (excluding history attribute)."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

# 2 metadata globals + history
echo "2."
CMD="$NCCMP -mghqf -x data1,data2,data3,data4,recdata,rec test1a.nc test1b.nc"
$CMD > stderr.tmp 2> /dev/null
ERR=`cat stderr.tmp`

if test `echo -n $ERR` = "1"; then :; else
    echo
    echo "metadata comparison failed on global history attribute values."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

# 3 nonrecord var metadata
echo "3."
CMD="$NCCMP -mqf -x recdata,rec test1a.nc test1b.nc"
$CMD > stderr.tmp 2> /dev/null
ERR=`cat stderr.tmp`

if test `echo -n $ERR` = "1"; then :; else
    echo
    echo "attribute comparison failed for non-record variables."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

# 4 nonrecord var values & dim lengths
echo "4."
CMD="$NCCMP -dqf -x recdata,rec test1a.nc test1b.nc"
$CMD > stderr.tmp 2> /dev/null
ERR=`cat stderr.tmp`

if test `echo -n $ERR` = "2"; then :; else
    echo
    echo "data comparison failed for non-record variables."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

# 5 record var metadata
echo "5."
CMD="$NCCMP -mqf -v recdata,rec test1a.nc test1b.nc"
$CMD > stderr.tmp 2> /dev/null
ERR=`cat stderr.tmp`

if test `echo -n $ERR` = "1"; then :; else
    echo
    echo "metadata comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

# 6 record var values
echo "6."
CMD="$NCCMP -dqf -v recdata,rec test1a.nc test1b.nc"
$CMD > stderr.tmp 2> /dev/null
ERR=`cat stderr.tmp`

if test `echo -n $ERR` = "1"; then :; else
    echo
    echo "data comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

# 7 diff. record names
echo "7."
CMD="$NCCMP -mdqf test2a.nc test2b.nc"
$CMD > stderr.tmp 2> /dev/null
ERR=`cat stderr.tmp`

if test `echo -n $ERR` = "1"; then :; else
    echo
    echo "metadata comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

# 8 diff. number of records
echo "8."
CMD="$NCCMP -mdqf test3a.nc test3b.nc"
$CMD > stderr.tmp 2> /dev/null
ERR=`cat stderr.tmp`

if test `echo -n $ERR` = "1"; then :; else
    echo
    echo "data comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

# success
exit 0
