#!/bin/sh
echo "57. Netcdf4 user defined compound type diffs."

CMD="$(./nccmp.sh) -mdf test15a.nc test15b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr57.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected compound type diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi
