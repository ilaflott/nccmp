#!/bin/sh

# find 'ncgen' command
NCGEN=`which ncgen 2> /dev/null`

if test "$?" = "0"; then :; else 
    echo "ERROR: ncgen command not found. Aborting tests..."
fi

#----------------------------
$NCGEN -o test1.nc test1.ncgen
./ncinfo test1.nc > tmp.stdout 2>&1
grep 'format=NC_FORMAT_CLASSIC' tmp.stdout > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "Expected to find format=NC_FORMAT_CLASSIC."
    exit 1
fi

#----------------------------
$NCGEN -k 2 -o test2.nc test2.ncgen
./ncinfo test2.nc > tmp.stdout 2>&1
grep 'format=NC_FORMAT_64BIT' tmp.stdout > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "Expected to find format=NC_FORMAT_64BIT."
    exit 1
fi

#----------------------------
$NCGEN -k 3 -o test3.nc test2.ncgen
./ncinfo test3.nc > tmp.stdout 2>&1
grep 'format=NC_FORMAT_NETCDF4' tmp.stdout > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "Expected to find format=NC_FORMAT_NETCDF4."
    exit 1
fi

#----------------------------
$NCGEN -k 4 -o test4.nc test2.ncgen
./ncinfo test4.nc > tmp.stdout 2>&1
grep 'format=NC_FORMAT_NETCDF4_CLASSIC' tmp.stdout > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "Expected to find format=NC_FORMAT_NETCDF4_CLASSIC."
    exit 1
fi

#----------------------------
echo "Test Passed"
