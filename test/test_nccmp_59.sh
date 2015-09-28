#!/bin/sh
echo "59. Netcdf4 user defined compound type tolerance diffs."

CMD="$(./nccmp.sh) -df -t 5 test15a.nc test15b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr59.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected compound type diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi
