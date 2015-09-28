#!/bin/bash
# Determine the proper "-k" format options for `ncgen`.

ncgen -k nc4 -o test1a.nc test1a.cdl > /dev/null 2>&1

if test "$?" = "0"; then 
    echo nc3 nc4
    exit
fi

ncgen -k hdf5 -o test1a.nc test1a.cdl

if test "$?" = "0"; then 
    echo 1 3
    exit
fi

echo UNKNOWN UNKNOWN
exit 1